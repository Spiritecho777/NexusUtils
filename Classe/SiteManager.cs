using Avalonia.Controls.Shapes;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace NexusUtils.Classe
{
    internal class SiteManager
    {
        private string appDirectory;
        private string filePath;
        private string filePathE;

        public SiteManager()
        {
            appDirectory = System.IO.Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "NexusUtilsData");

            if (!Directory.Exists(appDirectory))
            {
                Directory.CreateDirectory(appDirectory);
            }

            filePathE = System.IO.Path.Combine(appDirectory, "Site.dat");
            filePath = System.IO.Path.Combine(appDirectory, "Site_decrypted.dat");

            if (File.Exists(filePathE))
            {
                CryptoUtils encryptionManager = new CryptoUtils();
                encryptionManager.DecryptFromFile(filePathE, filePath);
            }
            else
            {
                CreateFile();
                Chiffrement();
            }
        }
        private void CreateFile()
        {
            File.WriteAllText(filePathE, string.Empty);
            File.WriteAllText(filePath, string.Empty);
        }

        private void Chiffrement()
        {
            CryptoUtils encryptionManager = new CryptoUtils();
            encryptionManager.EncryptionToFile(filePath, filePathE);
        }

        #region Traitements de donnée
        public bool FileExists()
        {
            return File.Exists(filePath);
        }

        public List<SiteItem> GetAllSites()
        {
            var sites = new List<SiteItem>();
            if (File.Exists(filePath))
            {
                var lines = File.ReadAllLines(filePath);
                foreach (var line in lines)
                {
                    var parts = line.Split(new[] { "||" }, StringSplitOptions.None);
                    if (parts.Length == 4)
                    {
                        sites.Add(new SiteItem
                        {
                            Name = parts[0],
                            Url = parts[1],
                            Credentials = parts[2],
                            Password = parts[3]
                        });
                    }
                }
            }
            return sites;
        }
        #endregion

        #region Manipulations de données
        public void AddSite(string Name, string Url)
        {
            string credentials = "";
            string password = "";

            var newline = $"{Name}||{Url}||{credentials}||{password}";

            List<string> line = File.ReadAllLines(filePath).ToList();
            line.Add(newline);
            File.WriteAllLines(filePath, line);

            Chiffrement();
        }

        public void AddCreds(string Iden, string password, SiteItem site)
        {
            var newline = $"{site.Name}||{site.Url}||{Iden}||{password}";
            List<string> lines = File.ReadAllLines(filePath).ToList();
            
            for (int i = 0; i < lines.Count; i++)
            {
                string[] parts = lines[i].Split("||",StringSplitOptions.None);

                if (parts[0] == site.Name)
                {
                    parts[2] = Iden;
                    parts[3] = password;
                }

                lines[i] = string.Join("||", parts);
            }

            File.WriteAllLines(filePath, lines);

            Chiffrement();
        }

        public void DeleteSite(SiteItem site)
        {
            List<string> lines = File.ReadAllLines(filePath).ToList();
            lines = lines.Where(line => !line.StartsWith(site.Name + "||")).ToList();
            File.WriteAllLines(filePath, lines);
            Chiffrement();
        }
        #endregion
    }
}
