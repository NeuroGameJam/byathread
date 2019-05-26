namespace Packager
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.Drawing;
    using System.IO;
    using System.Linq;
    using System.Text;
    using System.Text.RegularExpressions;
    using System.Web;
    using System.Xml;

    class Program
    {
        const string OUTPUT = "assets.dat";

        struct FileToAdd
        {
            public string filename;
            public long size;
        }

        static string output;

        static List<FileToAdd> files;

        static void Main(string[] args)
        {
            string searchPath = Directory.GetCurrentDirectory() + "\\assets-unpacked";
            string outputPath = Directory.GetCurrentDirectory() + "\\assets";

            files = new List<FileToAdd>();

            if (!Directory.Exists(searchPath))
            {
                Console.WriteLine("Sub-Directory assets-uncompressed not found.");
                return;
            }

            if (! Directory.Exists(outputPath))
                Directory.CreateDirectory(outputPath);            
            
            output = outputPath + "\\" + OUTPUT;

            if (File.Exists(output))
                File.Delete(output);            

            Console.WriteLine("Packing " + searchPath);

            if (File.Exists(output))
                File.Delete(output);

            ListFiles(searchPath);
            bool fileListChanged = false;

            string[] labelsFnt = new string[]{ "char id", "x", "y", "width", "height", "xoffset", "yoffset", "xadvance", "page", "chnl" };

            // pre-process all x files first
            var objFiles = files.Where(m => m.filename.EndsWith(".obj"));

            foreach (FileToAdd file in objFiles)
            {               
                string output = file.filename.Replace(".obj", ".x");

                if (! File.Exists(output) || new FileInfo(output).CreationTime < new FileInfo(file.filename).LastWriteTime)
                {
                    fileListChanged = true;

                    if (File.Exists(output))
                        File.Delete(output);

                    Console.WriteLine("Converting {0}", file.filename);

                    ObjModel model = new ObjModel(file.filename);

                    using (FileStream ofs = File.Create(output))
                    {
                        // write bytes
                        ofs.Write(model.buffer.ToArray(), 0, model.buffer.Count);
                    }
                }
            }

                ListFiles(searchPath);

            // Create the file.
            using (FileStream fs = File.Create(output))
            {
                foreach (FileToAdd file in files)
                {
                    // Ignore OBJ/MTL Files
                    if (file.filename.EndsWith(".obj") || file.filename.EndsWith(".mtl"))
                        continue;

                    string filename = file.filename.Replace(searchPath + "\\", String.Empty).Replace("\\", "/"); // cross platform stuff eheh;
                    string directory = String.Empty;
                    if (filename.Contains("/"))
                        directory = filename.Substring(0, filename.LastIndexOf("/"));

                    Console.WriteLine("Processing " + filename);

                    // write filename length
                    byte[] info = BitConverter.GetBytes((long)filename.Length);
                    fs.Write(info, 0, info.Length);

                    // write filename
                    if (file.filename.Contains("\\fonts\\") && file.filename.EndsWith(".xml"))
                        filename = filename.Replace(".xml", ".fnt");
                    info = Encoding.ASCII.GetBytes(filename);
                    fs.Write(info, 0, info.Length);

                    // FONT
                   if (file.filename.Contains("\\fonts\\") && file.filename.EndsWith(".xml"))
                    {                        
                        List<short> buffer = new List<short>();

                        XmlDocument doc = new XmlDocument();
                        doc.LoadXml(File.ReadAllText(file.filename));

                        XmlNodeList chars = doc.ChildNodes[1].ChildNodes[3].ChildNodes;
                        short description = short.Parse(doc.ChildNodes[1].ChildNodes[0].Attributes["size"].Value);

                        foreach (XmlNode item in chars)
                        {
                            string aux = HttpUtility.HtmlDecode(item.Attributes["id"].Value);
                            short id = Convert.ToInt16(aux[0]);
                            buffer.Add(id);

                            List<short> rect = item.Attributes["rect"].Value.Split(' ').Select(m => short.Parse(m)).ToList();
                            foreach (short v in rect)
                                    buffer.Add(v);

                            List<short> offset = item.Attributes["offset"].Value.Split(' ').Select(m => short.Parse(m)).ToList();                            
                            buffer.Add(offset.First());
                            buffer.Add((short)(description - offset.Last()));

                            buffer.Add(short.Parse(item.Attributes["advance"].Value));
                        }

                        // write size 
                        long fntSize = buffer.Count * 2;
                        info = BitConverter.GetBytes(fntSize); // 2 bytes each
                        fs.Write(info, 0, info.Length);

                        // write shorts
                        foreach (short value in buffer)
                        {
                            info = BitConverter.GetBytes(value);
                            fs.Write(info, 0, info.Length);
                        }
                    }
                    // PNG / LZW
                    else if (file.filename.EndsWith(".png"))
                    {
                        Bitmap bitmap = new Bitmap(file.filename);

                        List<byte> aux = new List<byte>();

                        for (int j = 0; j < bitmap.Height; j++)
                        {
                            for (int i = 0; i < bitmap.Width; i++)
                            {
                                Color pixel = bitmap.GetPixel(i, j);

                                // pre-multiplied
                                aux.Add((byte)(pixel.R * pixel.A / 255));
                                aux.Add((byte)(pixel.G * pixel.A / 255));
                                aux.Add((byte)(pixel.B * pixel.A / 255));

                                /* straight alpha
                                aux.Add(pixel.R);
                                aux.Add(pixel.G);
                                aux.Add(pixel.B);
                                */
                                aux.Add(pixel.A);
                            }
                        }

                        List<byte> compressed = new Lzw().Compress(aux);

                        List<byte> buffer = new List<byte>();

                        // width
                        info = BitConverter.GetBytes((short)bitmap.Width);
                        buffer.AddRange(info);

                        // height
                        info = BitConverter.GetBytes((short)bitmap.Height);
                        buffer.AddRange(info);

                        // add info
                        buffer.AddRange(compressed);

                        // write size 
                        info = BitConverter.GetBytes((long)buffer.Count);
                        fs.Write(info, 0, info.Length);

                        // write file
                        fs.Write(buffer.ToArray(), 0, buffer.Count);
                    }
                    // WAVE / LZW
                    else if (file.filename.EndsWith(".wav"))
                    {
                        List<byte> aux = File.ReadAllBytes(file.filename).ToList();

                        List<byte> compressed = new Lzw().Compress(aux);

                        // write size 
                        info = BitConverter.GetBytes((long)compressed.Count);
                        fs.Write(info, 0, info.Length);

                        // write file
                        fs.Write(compressed.ToArray(), 0, compressed.Count);
                    }                   

                    // shaders
                    else if (file.filename.EndsWith(".vs") || file.filename.EndsWith(".fs"))
                    {
                        String contents = File.ReadAllText(file.filename);

                        string pattern = "[^ -~\r\n]+";
                        Regex reg_exp = new Regex(pattern);
                        contents = reg_exp.Replace(contents, String.Empty);
                        contents += "\r\n";

                        byte[] buffer = Encoding.Convert(Encoding.Default, Encoding.ASCII, Encoding.Default.GetBytes(contents));

                        //string a = Encoding.ASCII.GetString(buffer);

                        // write size 
                        long filesize = buffer.Length;
                        info = BitConverter.GetBytes(filesize); // 2 bytes each
                        fs.Write(info, 0, info.Length);

                        // write file
                        fs.Write(buffer, 0, buffer.Length);
                    }
                    else
                    {
                        // write size
                        info = BitConverter.GetBytes(file.size);
                        fs.Write(info, 0, info.Length);

                        // write file
                        info = File.ReadAllBytes(file.filename);
                        fs.Write(info, 0, info.Length);
                    }
                }
            };

            Debug.WriteLine("done");
        }

        private static void ListFiles(string directory)
        {
            try
            {
                foreach (string f in Directory.GetFiles(directory))
                    AddFile(f);

                foreach (string d in Directory.GetDirectories(directory))
                    ListFiles(d);
            }
            catch (System.Exception except)
            {
                Console.WriteLine(except.Message);
            }
        }

        private static void AddFile(string filename)
        {            
            if (filename == output || filename.EndsWith("Packager.exe") || filename.Contains("autosave") || filename.Contains("bkp"))
                return;

            Console.WriteLine("Found " + filename);

            files.Add(new FileToAdd
            {
                filename = filename,
                size = new FileInfo(filename).Length
            });
        }
    }
}
