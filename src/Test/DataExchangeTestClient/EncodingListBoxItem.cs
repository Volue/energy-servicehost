using System.Text;

namespace DataExchangeTestClient
{
    public class EncodingListBoxItem
    {
        private readonly Encoding _encoding;
        private readonly string _name;

        public EncodingListBoxItem(Encoding encoding)
        {
            _encoding = encoding;
            _name = encoding.EncodingName;
        }

        public Encoding GetEncoding()
        {
            return _encoding;
        }

        public override string ToString()
        {
            return _name;
        }
    }
}
