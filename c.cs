using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Drawing;
using System.Threading;
using Timer = System.Windows.Forms.Timer;
using System.IO;

namespace Slideshow
{
    public class MainWindow : Form
    {
        // Interop code from http://www.codeproject.com/KB/cs/FullScreenDotNetApp.aspx
        [DllImport("user32.dll", EntryPoint = "GetSystemMetrics")]
        private static extern int GetSystemMetrics(int which);

        [DllImport("user32.dll")]
        private static extern void SetWindowPos(IntPtr hwnd, IntPtr hwndInsertAfter,
                     int x, int y, int width, int height, uint flags);

        private const int SM_CXSCREEN = 0;
        private const int SM_CYSCREEN = 1;
        private static IntPtr HWND_TOP = IntPtr.Zero;
        private const int SWP_SHOWWINDOW = 0x0040;

        private readonly Random _rng = new Random();
        private readonly List<string> _imageList = new List<string>();
        private Image _currentImage;
        private string _dir;

        public MainWindow(string dir)
        {
            _dir = dir;
            StartImageListPopulation();
            MakeTimer();

            WindowState = FormWindowState.Maximized;
            FormBorderStyle = FormBorderStyle.None;
            TopMost = true;
            SetWindowPos(Handle, HWND_TOP, 0, 0, ScreenWidth, ScreenHeight, SWP_SHOWWINDOW);
            BackColor = Color.Black;

        }

        private int ScreenWidth
        {
            get { return GetSystemMetrics(SM_CXSCREEN); }
        }

        private int ScreenHeight
        {
            get { return GetSystemMetrics(SM_CYSCREEN); }
        }

        private void MakeTimer()
        {
            var timer = new Timer();
            timer.Interval = 5000;
            timer.Tick += new EventHandler(timer_Tick);
            timer.Enabled = true;
        }

        private static IEnumerable<string> GetDirectoryDescendants(string path)
        {
            return Directory.GetFiles(path)
                .Concat(Directory.GetDirectories(path)
                            .SelectMany(directory => GetDirectoryDescendants(directory)));

        }

        private static bool IsJpeg(string file)
        {
            return Path.GetExtension(file).Equals(".jpg", StringComparison.InvariantCultureIgnoreCase);
        }

        private void PopulateImageList()
        {
            var descendants = GetDirectoryDescendants(_dir).Where(f => IsJpeg(f));
            foreach (var file in descendants)
            {
                _imageList.Add(file);
            }
        }

        private void StartImageListPopulation()
        {
            var thread = new Thread(PopulateImageList);
            thread.Priority = ThreadPriority.Lowest;
            thread.Start();
        }

        private string RandomImagePath()
        {
            var n = _imageList.Count;
            if (n == 0)
            {
                return null;
            }
            else
            {
                return _imageList[_rng.Next(n)];
            }
        }

        private Image RandomImage()
        {
            var path = RandomImagePath();

            if (path == null)
            {
                return null;
            }

            return Image.FromFile(path);
        }

        private void timer_Tick(object sender, EventArgs e)
        {
            _currentImage = RandomImage();
            Invalidate();
        }

        private Rectangle FitTo(Size imageDims, Size regionDims)
        {
            var scaling = Math.Min(1.0F, Math.Min((float)regionDims.Width / (float)imageDims.Width, (float)regionDims.Height / (float)imageDims.Height));
            var scaledImageDims = Scale(scaling, imageDims);
            var offset = Center(scaledImageDims, regionDims);

            return new Rectangle(offset, scaledImageDims);
        }

        private Point Center(Size imageDims, Size regionDims)
        {
            Func<int, int, int> average = (i, r) => (r / 2) - (i / 2);

            return new Point(average(imageDims.Width, regionDims.Width), average(imageDims.Height, regionDims.Height));
        }

        private Size Scale(float factor, Size dims)
        {
            return new Size((int)(dims.Width * factor), (int)(dims.Height * factor));
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            var g = e.Graphics;
            g.Clear(Color.Black);

            if (_currentImage == null)
            {
                g.DrawString("Working...", Font, Brushes.White, 800, 600);
            }
            else
            {
                var regionDims = new Size(ScreenWidth, ScreenHeight);
                var imageDims = new Size(_currentImage.Width, _currentImage.Height);

                var destination = FitTo(imageDims, regionDims);

                g.DrawImage(_currentImage, destination);
            }
        }

    }

    public class Program
    {
        static void Main(string[] args)
        {
            Application.Run(new MainWindow(args[0]));
        }
    }
}