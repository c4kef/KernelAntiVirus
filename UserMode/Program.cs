using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace KsDumperClient
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            var original = new List<string>();
            original.AddRange(File.ReadAllLines("original.txt"));
            var found = File.ReadAllLines("found.txt");
            foreach (var f in found)
                original.RemoveAll(line => line.ToLower().Contains(f.ToLower()));

            File.WriteAllLines("reversed.txt", original.ToArray());
            /*Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Dumper());*/
        }
    }
}
