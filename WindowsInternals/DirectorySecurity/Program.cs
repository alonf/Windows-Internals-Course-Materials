using System;
using System.IO;
using System.Security.AccessControl;

namespace DirectorySecurity
{
    class Program
    {
        static void Main()
        {
            const string dir = @"C:\test";

            if (!Directory.Exists(dir))
                Directory.CreateDirectory(dir);

            System.Security.AccessControl.DirectorySecurity ds = Directory.GetAccessControl(dir);
            ds.AddAccessRule(new FileSystemAccessRule("Guest", FileSystemRights.Read, AccessControlType.Allow));
            Directory.SetAccessControl(dir, ds);

            Console.WriteLine(ds.GetSecurityDescriptorSddlForm(AccessControlSections.All));
        }
    }
}
