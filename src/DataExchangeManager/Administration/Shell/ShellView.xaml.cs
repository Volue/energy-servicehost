using System;
using System.Windows;
using System.Windows.Interop;

namespace DataExchange.Administration.Shell
{
    /// <summary>
    /// Interaction logic for ShellView.xaml
    /// </summary>
    public partial class ShellView : Window
    {
        public ShellView()
        {
            InitializeComponent();
        }

        protected override void OnSourceInitialized(EventArgs e)
        {
            base.OnSourceInitialized(e);
            HwndSource source = PresentationSource.FromVisual(this) as HwndSource;
            source.AddHook(WndProc);
        }

        // WndProc is inherited from the Window base class
        // Receives messages that are posted by App.xaml.cs in case a duplicate process has been started
        private IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            if (msg == NativeMethods.WM_SHOWME)
            {
                ShowApplicationOnTop();
            }

            return IntPtr.Zero;
        }

        private void ShowApplicationOnTop()
        {
            if (WindowState == WindowState.Minimized)
            {
                WindowState = WindowState.Normal;
            }

            // Store the previously used value
            bool top = Topmost;

            // Show the user that the application is started by putting the window on top
            Topmost = true;

            // And put the old value back here
            Topmost = top;
        }

    }

    
}
