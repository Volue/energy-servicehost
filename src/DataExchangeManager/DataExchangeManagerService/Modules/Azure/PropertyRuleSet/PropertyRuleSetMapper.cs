using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Xml.Linq;
using Powel.Extensions;
using Powel.Icc.Common;
using Powel.Icc.Diagnostics;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Azure.PropertyRuleSet;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Azure
{
    public class PropertyRuleSetMapper
    {
        private readonly IServiceEventLogger _logger;
        private readonly string _fileName;
        private static IList<RuleConstant> _ruleConstants;
        private PropertyRuleSets _ruleSets;
        public PropertyRuleSets RuleSets { get { return _ruleSets; } }

        public PropertyRuleSetMapper(string fileName, IServiceEventLogger logger)
        {
            _fileName = fileName;
            _logger = logger;
        }

        private XDocument LoadRuleSetFile()
        {
            try
            {
                XDocument xmlDoc;
                using (var stream = new StreamReader(_fileName, true))
                {
                    xmlDoc = XDocument.Load(stream);
                    var root = xmlDoc.Root;
                    if (root == null)
                    {
                        _logger.LogToEventLog(string.Format("Ruleset file {0} is corrupted", _fileName), EventLogEntryType.Error);
                        throw new InvalidDataException("Report xml is corrupted.");
                    }
                }
                return xmlDoc;
            }
            catch (FileNotFoundException)
            {
                _logger.LogToEventLog(string.Format("Rulefile {0} was not found", _fileName), EventLogEntryType.Error);
                return null;
            }
            catch (Exception exception)
            {
                _logger.LogCritical(exception,true);    // .LogToEventLog(string.Format("Failed when loading rulefile {0}.", _fileName), );
                return null;
            }
        }

        public PropertyRuleSets Map()
        {
            var xDocument = LoadRuleSetFile();
            if (xDocument == null)
                return null;

            var root = xDocument.Root;
            if (root == null)
            {
                _logger.LogToEventLog("Found no data in rulefile", EventLogEntryType.Error);
                return null;
            }

            var rulesets = new List<Rule>();
            var manager = new Volue.Secrets.Storage.SecretsManager(IccConfiguration.Data.OracleConnectionString);
            foreach (var xElement in root.Elements())
            {
                if (xElement.Name.ToString().Equals("RuleConstants"))
                {
                    try
                    {
                        var consts = xElement.Elements().Select(ruleConstant => new RuleConstant() {
                            Value = ruleConstant.Value.Contains(@"[$SECRET:") ? manager.Decrypt(ruleConstant.Value) : ruleConstant.Value,
                            Name = ruleConstant.Attribute("name").Value
                        }).ToList();
                        _ruleConstants = consts;
                    }
                    catch (Exception E)
                    {
                        _logger.LogCritical(E,true);
                    }
                }
                else if (xElement.Name.ToString().Equals("Rule"))
                {
                    rulesets.Add(CreateRule(xElement.Attribute("name").Value, xElement.Element("Select"),
                        xElement.Element("Properties")));
                }
                else
                {
                    throw new InvalidDataException(string.Format("UnknownElement{0}", xElement.Name.ToString()));
                }
            }
            _ruleSets = new PropertyRuleSets() { Rules = rulesets };
            return _ruleSets;
        }

        private Rule CreateRule(string name, XElement select, XElement properties)
        {
            var r = new Rule { Name = name, Select = CreateSelect(select.Element("MetaData"),select.Element("Payload")) };
            properties.Elements().ForEach(prop => r.Properties[prop.Attribute("key").Value] = prop.Value);
            return r;
        }

        private SelectEntity CreateSelect(XElement metadata, XElement payload)
        {
            var s = new SelectEntity { Payload = payload == null ? string.Empty : payload.Value};
            if (metadata != null)
                metadata.Elements().ForEach(prop => s.MetaData[prop.Attribute("key").Value] = prop.Value);
            return s;
        }

        public string MapValue(string value)
        {
            var fParams = new Regex(@"\$CONST\((\w+)\)");
            var fMatch = fParams.Match(value);
            if (!fMatch.Success)
                return value;

            var pattern = fMatch.Groups[0].Value;
            var constName = fMatch.Groups[1].Value;
            foreach (var ruleConstant in _ruleConstants.Where(ruleConstant => ruleConstant.Name.Equals(constName)))
            {
                return MapValue(value.Replace(pattern, ruleConstant.Value));
            }
            return value;
        }
    }
}
