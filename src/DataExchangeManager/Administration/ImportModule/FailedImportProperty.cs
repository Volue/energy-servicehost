namespace DataExchange.Administration.ImportModule
{
    public class FailedImportProperty
    {
        private readonly string _originalPropertyValue;
        public FailedImportProperty(FailedImportPropertyName name, string value)
        {
            PropertyName = name;
            PropertyValue = value;
            _originalPropertyValue = value;
        }
        public FailedImportPropertyName PropertyName { get; private set; }
        public string PropertyValue { get; set; }

        public override string ToString()
        {
            return PropertyValue;
        }

        public bool IsValueModified
        {
            get { return PropertyValue != _originalPropertyValue; }
        }

        public void RevertChanges()
        {
            PropertyValue = _originalPropertyValue;
        }
    }
}
