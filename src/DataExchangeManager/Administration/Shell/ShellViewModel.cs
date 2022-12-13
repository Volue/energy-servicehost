using DataExchange.Administration.Shell.Properties;

namespace DataExchange.Administration.Shell
{
    public class ShellViewModel
    {
        public ShellViewModel()
        {
            Title = Resources.Title;
            MinWidth = 500;
            MinHeight = 300;
            Width = 800;
            Height = 600;
        }

        public string Title { get; set; }

        public int MinWidth { get; set; }

        public int MinHeight { get; set; }

        public int Width { get; set; }

        public int Height { get; set; }
    }
}
