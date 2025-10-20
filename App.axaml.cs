using Avalonia;
using Avalonia.Controls.ApplicationLifetimes;
using Avalonia.Markup.Xaml;
using NexusUtils.Popup;
using System;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using Xilium.CefGlue;

namespace NexusUtils
{
    public partial class App : Application
    {
        private static Mutex? mutex;
        private static bool mutexAcquired = false;

        public App()
        {
            AppDomain.CurrentDomain.ProcessExit += CurrentDomain_ProcessExit; // ProcessExit appelé même si l'application est tuée.
        }

        public override void Initialize()
        {
            AvaloniaXamlLoader.Load(this);
        }

        public override void OnFrameworkInitializationCompleted()
        {
            bool isNewInstance = AcquireCrossPlatformMutex("NexusUtils_Mutex");
            if (!isNewInstance) 
            {
                if (ApplicationLifetime is IClassicDesktopStyleApplicationLifetime desktop)
                {
                    new AlerteWindow("Une autre instance de l'application est déjà en cours d'exécution.")
                        .ShowDialog(desktop.MainWindow);
                    desktop.Shutdown();
                }
                return;
            }
            string cefPath = RuntimeInformation.IsOSPlatform(OSPlatform.Windows)
                ? Path.Combine(AppContext.BaseDirectory, "cef", "winx64")
                : Path.Combine(AppContext.BaseDirectory, "cef", "linx64");
            
            CefRuntime.Load(cefPath);

            // CEF init
            var mainArgs = new CefMainArgs(Environment.GetCommandLineArgs());
            var exitCode = CefRuntime.ExecuteProcess(mainArgs, null, IntPtr.Zero);
            if (exitCode >= 0)
            {
                Environment.Exit(exitCode);
            }

            CefRuntime.Load();
            var settings = new CefSettings
            {
                WindowlessRenderingEnabled = true,
                MultiThreadedMessageLoop = true,
                UserAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36",
                CachePath = Path.Combine(AppContext.BaseDirectory, "cef_cache"),
                RootCachePath = Path.Combine(AppContext.BaseDirectory, "cef_root"),
                PersistSessionCookies = true,
                PersistUserPreferences = true
            };

            var app = new CustomCefApp();

            CefRuntime.Initialize(mainArgs, settings, null);

            if (ApplicationLifetime is IClassicDesktopStyleApplicationLifetime desktopLifetime)
            {
                desktopLifetime.Exit += OnAppExit;
                desktopLifetime.MainWindow = new MainWindow();
            }

            base.OnFrameworkInitializationCompleted();
        }

        private bool AcquireCrossPlatformMutex(string mutexName)
        {
            if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                mutex = new Mutex(true, mutexName, out bool isNewInstance);
                return isNewInstance;
            }
            else
            {
                string lockPath = Path.Combine(Path.GetTempPath(), mutexName + ".lock");

                try
                {
                    if (File.Exists(lockPath))
                    {
                        return false;
                    }

                    File.WriteAllText(lockPath, Process.GetCurrentProcess().Id.ToString());
                    AppDomain.CurrentDomain.ProcessExit += (_, _) =>
                    {
                        try { File.Delete(lockPath); } catch { }
                    };
                    return true;
                }
                catch
                {
                    return false;
                }
            }
        }

        #region Manipulation faite suite a la fermeture - crash du logiciel

        public class CustomCefApp : CefApp
        {
            protected override void OnBeforeCommandLineProcessing(string processType, CefCommandLine commandLine)
            {
                commandLine.AppendSwitch("disable-gpu");
                commandLine.AppendSwitch("disable-gpu-compositing");
                commandLine.AppendSwitch("enable-begin-frame-scheduling");
                commandLine.AppendSwitch("disable-gpu-vsync");
                //commandLine.AppendSwitchWithValue("use-gl", "angle");
                //commandLine.AppendSwitchWithValue("log-severity", "warning");
            }
        }

        // Méthode appelée lorsque l'application se termine proprement
        private void OnAppExit(object? sender, ControlledApplicationLifetimeExitEventArgs e)
        {
            DeleteFile();
            if (mutexAcquired && mutex is not null)
            {
                try { mutex.ReleaseMutex(); } catch { }
            }
        }

        // Méthode appelée lorsque le processus est tué ou termine
        private void CurrentDomain_ProcessExit(object sender, EventArgs e)
        {
            DeleteFile();
            if (mutexAcquired && mutex is not null)
            {
                try { mutex.ReleaseMutex(); } catch { }
            }
        }

        private void DeleteFile()
        {
            string appDirectory = System.IO.Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "NexusUtilsData");
            string filePath = System.IO.Path.Combine(appDirectory, "Site_decrypted.dat");

            if (File.Exists(filePath))
            {
                File.Delete(filePath);
            }

            string filePath2 = System.IO.Path.Combine(appDirectory, "Site.dat");

            if (File.Exists(filePath2))
            {
                long fileSize = new FileInfo(filePath2).Length;

                if (fileSize == 0 && File.Exists(filePath2))
                {
                    File.Delete(filePath2);
                }
            }
        }
        #endregion
    }
}