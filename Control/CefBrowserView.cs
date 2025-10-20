using Avalonia;
using Avalonia.Controls;
using Avalonia.Media.Imaging;
using Avalonia.Platform;
using Avalonia.Threading;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Xilium.CefGlue;
using Xilium.CefGlue.Common.Handlers;

namespace NexusUtils.Control
{
    public class CefBrowserView : UserControl
    {
        private CefBrowser? _browser;
        private readonly string _initialUrl;
        private readonly Image _image = new();
        private WriteableBitmap? _bitmap;

        public CefBrowserView(string url)
        {
            _initialUrl = url;
            Content = _image;
        }

        protected override void OnAttachedToVisualTree(VisualTreeAttachmentEventArgs e)
        {
            base.OnAttachedToVisualTree(e);

            var windowInfo = CefWindowInfo.Create();
            windowInfo.SetAsWindowless(IntPtr.Zero, true);

            var client = new _CefClient(this);
            var settings = new CefBrowserSettings();

            CefBrowserHost.CreateBrowser(windowInfo, client, settings, _initialUrl);
        }

        public void SetBrowser(byte[] buffer, int width, int height)
        {
            Dispatcher.UIThread.Post(() =>
            {
                _bitmap = new WriteableBitmap(new PixelSize(width, height), new Vector(96, 96), Avalonia.Platform.PixelFormat.Bgra8888, Avalonia.Platform.AlphaFormat.Premul);
                using var fb = _bitmap.Lock();
                Marshal.Copy(buffer, 0, fb.Address, buffer.Length);
                _image.Source = _bitmap;
            });
        }

        public void Navigate(string url)
        {
            _browser?.GetMainFrame().LoadUrl(url);
        }

        public void SetBrowser(CefBrowser browser)
        {
            _browser = browser;
        }

        public void SetBitmap(byte[] buffer, int width, int height)
        {
            var wb = new WriteableBitmap(new PixelSize(width, height), new Vector(96, 96), PixelFormat.Bgra8888, AlphaFormat.Premul);
            using var fb = wb.Lock();
            System.Runtime.InteropServices.Marshal.Copy(buffer, 0, fb.Address, buffer.Length);
            _image.Source = wb;
        }

    }

    #region classe ajouté
    public class _CefClient : CefClient
    {
        private readonly CefBrowserView _view;
        private readonly _RenderHandler _renderHandler;

        public _CefClient(CefBrowserView view)
        {
            _view = view;
            _renderHandler = new _RenderHandler(view);
        }

        protected override CefRenderHandler GetRenderHandler() => _renderHandler;
        protected override CefLifeSpanHandler GetLifeSpanHandler() => new _LifeSpanHandler(_view);
        protected override CefLoadHandler GetLoadHandler() => new _LoadHandler();
    }

    //Sa c'est beucoup trop bas niveau je n'ai absolument rien compris
    public class _LifeSpanHandler : CefLifeSpanHandler
    {
        private readonly CefBrowserView _view;
        public _LifeSpanHandler(CefBrowserView view)
        {
            _view = view;
        }
        protected override void OnAfterCreated(CefBrowser browser)
        {
            _view.SetBrowser(browser);
        }
    }
    
    public class _LoadHandler : CefLoadHandler
    {
        protected override void OnLoadEnd(CefBrowser browser, CefFrame frame, int httpStatusCode)
        {
            // Gérer les erreurs de chargement si nécessaire
        }
    }

    public class _RenderHandler : CefRenderHandler
    {
        private readonly CefBrowserView _view;

        public _RenderHandler(CefBrowserView view)
        {
            _view = view;
        }

        protected override void OnPaint(CefBrowser browser, CefPaintElementType type, CefRectangle[] dirtyRects, nint buffer, int width, int height)
        {
            var size = width * height * 4;
            var managedBuffer = new byte[size];
            System.Runtime.InteropServices.Marshal.Copy(buffer, managedBuffer, 0, size);
            Dispatcher.UIThread.Post(() =>
            {
                _view.SetBitmap(managedBuffer, width, height);
                _view.SetBrowser(browser);
            });
        }

        protected override void GetViewRect(CefBrowser browser, out CefRectangle rect)
        {
            rect = new CefRectangle(0, 0, 1024, 768); // à adapter dynamiquement
            //return true;
        }

        protected override bool GetScreenInfo(CefBrowser browser, CefScreenInfo screenInfo)
        {
            screenInfo.Rectangle = new CefRectangle(0, 0, 1024, 768);
            screenInfo.AvailableRectangle = screenInfo.Rectangle;
            screenInfo.DeviceScaleFactor = 1;
            screenInfo.Depth = 32;
            screenInfo.DepthPerComponent = 8;
            screenInfo.IsMonochrome = false;
            return true;
        }

        protected override CefAccessibilityHandler? GetAccessibilityHandler()
        {
            return null;
        }

        protected override void OnAcceleratedPaint(CefBrowser browser, CefPaintElementType type, CefRectangle[] dirtyRects, nint sharedHandle)
        {
            // Ignoré pour le rendu software
        }

        protected override void OnImeCompositionRangeChanged(CefBrowser browser, CefRange selectedRange, CefRectangle[] characterBounds)
        {
            // Ignoré sauf si tu gères les IME (claviers asiatiques)
        }

        protected override void OnPopupSize(CefBrowser browser, CefRectangle rect)
        {
            // Ignoré sauf si tu veux gérer les popups manuellement
        }

        protected override void OnScrollOffsetChanged(CefBrowser browser, double x, double y)
        {
            // Ignoré sauf si tu veux synchroniser le scroll
        }
    }
    #endregion
}
