using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Security.Cryptography;

namespace NexusUtils.Classe
{
    internal class GuidMachine
    {
        public static string GetMachineGuid()
        {
            string systemDrive = Path.GetPathRoot(Environment.SystemDirectory);
            DriveInfo? drive = DriveInfo.GetDrives()
                .FirstOrDefault(d => d.IsReady && d.Name == systemDrive);

            string raw = string.Join("|", new[]
            {
                Environment.UserName,
                Environment.OSVersion.VersionString,
                RuntimeInformation.OSArchitecture.ToString(),
                RuntimeInformation.OSDescription,
                drive?.DriveFormat ?? "UnknownFormat",
                drive?.VolumeLabel ?? "UnknownLabel",
                drive?.Name ?? "UnknownDrive"
            });

            using var sha = SHA256.Create();
            byte[] hash = sha.ComputeHash(Encoding.UTF8.GetBytes(raw));
            return Convert.ToHexString(hash); // 64-char hex string
        }
    }
}
