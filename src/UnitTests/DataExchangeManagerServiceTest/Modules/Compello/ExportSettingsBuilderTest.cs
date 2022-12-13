using Moq;
using NUnit.Framework;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Abstract;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Builders;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerService.Modules.Compello.Model;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeManagerServiceTest.Modules.Compello
{
    [TestFixture]
    public class ExportSettingsBuilderTest
    {
        [Test]
        public void Build_MessageIsValid_BuildsCorrectExportSettings()
        {
            const string exportHostName = "exportHostName";
            const int exportPortNumber = 444;
            const string  apiKey = "apiKey";
            const int heartBeatInterval = 123;
            const int restartInterval = 456;
            const string _dummyMessageData = "DummyMessageData";
            var configSystemSettings = new Settings(exportHostName, exportPortNumber, apiKey, heartBeatInterval, restartInterval);
            var settingsProviderMock = new Mock<ISettingsProvider>();

            settingsProviderMock
                .Setup(sp => sp.GetSettings())
                .Returns(configSystemSettings);
            var message = new DataExchangeExportMessage()
            {
                MessageLogId = 1,
                MessageReference = "DummyMessageReference",
                RoutingAddress = "COMPELLO:"
            };
            message.SetMessageData(_dummyMessageData,null);
            var sut = new ExportSettingsBuilder(settingsProviderMock.Object);

            var result = sut.Build(message);

            Assert.AreEqual(apiKey, result.ApiKey);
            Assert.AreEqual(heartBeatInterval, result.HeartbeatInterval);
            Assert.AreEqual(restartInterval, result.RestartInterval);
            Assert.AreEqual(exportHostName, result.HostAddress);
            Assert.AreEqual(exportPortNumber, result.Port);
            settingsProviderMock
                .Verify(sp => sp.GetRoutingAddressForImport(), Times.Never());
            message.DeleteMessageData();
        }
    }
}