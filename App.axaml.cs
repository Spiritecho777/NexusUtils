using Avalonia;
using Avalonia.Controls.ApplicationLifetimes;
using Avalonia.Markup.Xaml;
using Avalonia.Metadata;
using NexusUtils.Popup;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
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
            InitializeCef();

            bool isNewInstance = AcquireCrossPlatformMutex("NexusUtils_Mutex");

            if (ApplicationLifetime is IClassicDesktopStyleApplicationLifetime desktop)
            {
                desktop.MainWindow = new MainWindow();

                desktop.MainWindow.Opened += async (_, __) =>
                {
                    if (!isNewInstance)
                    {
                        var alert = new AlerteWindow("Une autre instance de l'application est déjà en cours d'exécution.");
                        await alert.ShowDialog(desktop.MainWindow);
                        desktop.Shutdown();
                    }
                };

                desktop.Exit += OnAppExit;
            }

            base.OnFrameworkInitializationCompleted();
        }
            //string cefPath = RuntimeInformation.IsOSPlatform(OSPlatform.Windows)

        // CEF init
        private void InitializeCef() 
        {
            try
            {
                var mainArgs = new CefMainArgs(Environment.GetCommandLineArgs());
                var app = new CustomCefApp();

                var exitCode = CefRuntime.ExecuteProcess(mainArgs, app, IntPtr.Zero);
                if (exitCode >= 0)
                {
                    Environment.Exit(exitCode);
                }

                CefRuntime.Load();
                var settings = new CefSettings
                {
                    WindowlessRenderingEnabled = false,
                    MultiThreadedMessageLoop = false,
                    UserAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36",
                    RootCachePath = Path.Combine(AppContext.BaseDirectory, "cef_root"),
                    CachePath = Path.Combine(AppContext.BaseDirectory, "cef_root/cache"),
                    PersistSessionCookies = true,
                    PersistUserPreferences = true,
                    NoSandbox = true,
                    LogSeverity = CefLogSeverity.Warning,
                    CommandLineArgsDisabled = false
                };

                Directory.CreateDirectory(settings.RootCachePath);
                Directory.CreateDirectory(settings.CachePath);

                CefRuntime.Initialize(mainArgs, settings, app, IntPtr.Zero);
            }
            catch (Exception ex)
            {
                Debug.WriteLine("CEF Initialization Error: " + ex.Message);
                throw;
            }
            
            /*if (ApplicationLifetime is IClassicDesktopStyleApplicationLifetime desktopLifetime)
            {
                desktopLifetime.Exit += OnAppExit;
                desktopLifetime.MainWindow = new MainWindow();
            }*/
        }

        private bool AcquireCrossPlatformMutex(string mutexName)
        {
            if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                mutex = new Mutex(true, mutexName, out bool isNewInstance);
                mutexAcquired = isNewInstance;
                return isNewInstance;
            }
            else
            {
                string lockPath = Path.Combine(Path.GetTempPath(), mutexName + ".lock");

                try
                {
                    if (File.Exists(lockPath))
                    {
                        // Vérifier si le processus existe toujours
                        try
                        {
                            string pidStr = File.ReadAllText(lockPath);
                            if (int.TryParse(pidStr, out int pid))
                            {
                                Process.GetProcessById(pid);
                                // Le processus existe, on ne peut pas acquérir le mutex
                                return false;
                            }
                        }
                        catch
                        {
                            // Le processus n'existe plus, supprimer le fichier
                            File.Delete(lockPath);
                        }
                    }

                    File.WriteAllText(lockPath, Process.GetCurrentProcess().Id.ToString());
                    mutexAcquired = true;

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
                // Switches communs
                commandLine.AppendSwitch("disable-gpu-vsync");
                commandLine.AppendSwitch("disable-gpu-shader-disk-cache");
                //commandLine.AppendSwitch("disable-software-rasterizer");

                // Switches spécifiques Linux pour résoudre les problèmes GPU
                if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
                {
                    // CRITICAL - Single process mode pour WSL
                    commandLine.AppendSwitch("single-process");
                    commandLine.AppendSwitch("no-zygote");
                    commandLine.AppendSwitch("no-sandbox");
                    commandLine.AppendSwitch("disable-setuid-sandbox");

                    // GPU complètement désactivé
                    commandLine.AppendSwitch("disable-gpu");
                    commandLine.AppendSwitch("disable-gpu-compositing");
                    commandLine.AppendSwitch("disable-accelerated-2d-canvas");
                    commandLine.AppendSwitch("disable-3d-apis");

                    // Rendu software
                    commandLine.AppendSwitch("use-gl", "swiftshader");
                    commandLine.AppendSwitch("disable-software-rasterizer");

                    // Désactiver features problématiques
                    commandLine.AppendSwitch("disable-dev-shm-usage");
                    commandLine.AppendSwitch("disable-features=VizDisplayCompositor");
                    commandLine.AppendSwitch("disable-features=NetworkService");
                    commandLine.AppendSwitch("disable-features=AudioServiceOutOfProcess");

                    // Audio
                    commandLine.AppendSwitch("disable-audio-output");
                    commandLine.AppendSwitch("mute-audio");

                    // Éviter les crashs X11
                    commandLine.AppendSwitch("disable-gpu-sandbox");
                    commandLine.AppendSwitch("disable-gpu-watchdog");
                    commandLine.AppendSwitch("disable-gpu-vsync");
                    commandLine.AppendSwitch("disable-background-timer-throttling");
                    commandLine.AppendSwitch("disable-backgrounding-occluded-windows");
                    commandLine.AppendSwitch("disable-breakpad");

                    // Logging
                    commandLine.AppendSwitch("log-severity");
                }
                else
                {
                    // Windows
                    commandLine.AppendSwitch("disable-gpu-compositing");
                    commandLine.AppendSwitch("enable-begin-frame-scheduling");
                    commandLine.AppendSwitch("disable-gpu-sandbox");
                    commandLine.AppendSwitch("disable-gpu-driver-bug-workarounds");
                    commandLine.AppendSwitch("disable-gpu-process-crash-limit");
                    commandLine.AppendSwitch("disable-gpu-watchdog");
                    commandLine.AppendSwitch("disable-gpu-memory-buffer-compositor-resources");
                    commandLine.AppendSwitch("disable-gpu-program-cache");
                    commandLine.AppendSwitch("disable-gpu-rasterization");
                    commandLine.AppendSwitch("disable-gpu-threaded-texture-mailbox");
                    commandLine.AppendSwitch("disable-gpu-early-init");
                    commandLine.AppendSwitch("disable-surfaces");
                }
            }
        }

        // Méthode appelée lorsque l'application se termine proprement
        private void OnAppExit(object? sender, ControlledApplicationLifetimeExitEventArgs e)
        {
            CleanupCef();
            DeleteFile();
            ReleaseMutex();
        }

        // Méthode appelée lorsque le processus est tué ou termine
        private void CurrentDomain_ProcessExit(object sender, EventArgs e)
        {
            CleanupCef();
            DeleteFile();
            ReleaseMutex();
        }

        private void CleanupCef()
        {
            try
            {
                CefRuntime.Shutdown();
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"Erreur lors du nettoyage CEF: {ex.Message}");
            }
        }

        private void ReleaseMutex()
        {
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