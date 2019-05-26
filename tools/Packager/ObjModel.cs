namespace Packager
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Text.RegularExpressions;

    class Vector3
    {
        public float x;
        public float y;
        public float z;
    }

    class Vector3UShort
    {
        public ushort x;
        public ushort y;
        public ushort z;
    }

    class VColor
    {
        public byte r;
        public byte g;
        public byte b;
        public byte a;
    }

    class Indexes
    {
        public Vector3UShort[] positions = new Vector3UShort[3];
    }

    class ObjModel
    {
        private List<Vector3> v; // vertices - v
        private List<Vector3> vn; // vertex normals - vn
        private List<Vector3> vt; // texture coords - vt
        private List<Indexes> f; // faces - vf
        private List<VColor> c; // colors - from material name - times faces

        public List<byte> buffer;

        public ObjModel(String filename)
        {
            String contents = File.ReadAllText(filename);

            v = new List<Vector3>();
            vn = new List<Vector3>();
            vt = new List<Vector3>();
            f = new List<Indexes>();
            c = new List<VColor>();
            List<VColor> colors = new List<VColor>();

            buffer = new List<byte>();

            contents = Regex.Replace(contents, @" +", " "); // replace multiple spaces with one

            // PARSE
            string[] lines = contents.Split('\n').ToList().Select(m => m.Trim()).ToArray();

            VColor color = new VColor();            

            foreach (string line in lines)
            {
                if (String.IsNullOrEmpty(line))
                    continue;

                if (line.StartsWith("#"))
                    continue;

                if (line.StartsWith("usemtl"))
                {
                    color = new VColor();

                    if (line == "usemtl default")
                        continue;

                    if (line.StartsWith("usemtl Color.") && line.Contains(","))
                    {
                        string[] colorParts = line.Split('.');
                        string[] colorFractions = colorParts[1].Split(',');
                        color.r = byte.Parse(colorFractions[0]);
                        color.g = byte.Parse(colorFractions[1]);
                        color.b = byte.Parse(colorFractions[2]);
                        color.a = byte.Parse(colorFractions[3]);
                    }
                    else color = new VColor { r = 0, g = 0, b = 0, a = 255 };

                    colors.Add(color);
                    continue;
                }

                string[] parts = line.Split(' ');

                switch (parts[0])
                {
                    case "v":
                    case "vn":
                    case "vt":
                        Vector3 aux = new Vector3()
                        {
                            x = float.Parse(parts[1]),
                            y = float.Parse(parts[2]),
                            z = parts.Length > 3 ? float.Parse(parts[3]) : 0
                        };

                        switch (parts[0])
                        {
                            case "v": v.Add(aux); break;
                            case "vn": vn.Add(aux); break;
                            case "vt":
                                vt.Add(aux);
                                break;
                        }

                        break;

                    case "f":

                        Indexes index = new Indexes();

                        for (int h = 1; h <= 3; h++)
                        {
                            string[] subparts = parts[h].Split('/').ToArray();
                            
                            Vector3UShort auxf = new Vector3UShort()
                            {
                                x = ushort.Parse(subparts[0]),                                
                                y = subparts.Length < 2 || subparts[1].Length == 0 ? (ushort)0 : ushort.Parse(subparts[1]),
                                z = subparts.Length < 3 || subparts[2].Length == 0 ? (ushort)0 : ushort.Parse(subparts[2])
                            };

                            index.positions[h - 1] = auxf;
                        }

                        f.Add(index);

                        c.Add(color);

                        break;
                }
            }

            // reorder
            List<Vector3> vertices = new List<Vector3>();
            List<Vector3> uvs = new List<Vector3>();
            List<Vector3> normals = new List<Vector3>();

            float texturePosModifier = (1.0f / colors.Count);

            int materialIndex = 0;
            int keyIndex = 0;
            Vector3 vaux;
            foreach (Indexes index in f)
            {
                foreach (Vector3UShort part in index.positions)
                {
                    vertices.Add(v[part.x - 1]);

                    if (vt.Count > 0)
                    {
                        vaux = new Vector3() { x = vt[part.y - 1].x, y = 1 - vt[part.y - 1].y, z = vt[part.y - 1].z };

                        if (colors.Count > 1)
                        {
                            materialIndex = colors.IndexOf(c[keyIndex]);

                            //vaux.x = vaux.x * texturePosModifier / 100.0f + materialIndex * texturePosModifier + (vaux.x == 1.0f ? 0.0f : .01f);
                            vaux.y = .5f;
                            vaux.z = 0;
                            vaux.x = texturePosModifier * materialIndex + texturePosModifier / 2;
                        }

                        uvs.Add(vaux);
                    }

                    if (vn.Count > 0)
                        normals.Add(vn[part.z - 1]);
                }

                keyIndex++;
            }            

            // WRITE TO BUFFER
            buffer.AddRange(BitConverter.GetBytes((ushort)vertices.Count));
            foreach (Vector3 vector in vertices)
                AddVector(vector);

            if (vertices.Count > 0)
            {
                Vector3 min, max, size;

                min = new Vector3()
                {
                    x = vertices.Min(m => m.x),
                    y = vertices.Min(m => m.y),
                    z = vertices.Min(m => m.z)
                };

                max = new Vector3()
                {
                    x = vertices.Max(m => m.x),
                    y = vertices.Max(m => m.y),
                    z = vertices.Max(m => m.z)
                };

                size = new Vector3()
                {
                    x = Math.Abs(max.x - min.x),
                    y = Math.Abs(max.y - min.y),
                    z = Math.Abs(max.z - min.z)
                };

                AddVector(size);
            }

            buffer.AddRange(BitConverter.GetBytes((ushort)uvs.Count));
            foreach (Vector3 vector in uvs)
                AddVector(vector);

            buffer.AddRange(BitConverter.GetBytes((ushort)normals.Count));
            foreach (Vector3 vector in normals)
                AddVector(vector);

            buffer.AddRange(BitConverter.GetBytes((ushort)colors.Count));
            foreach (VColor c in colors)
                AddVector(c);
        }

        private void AddVector(Vector3 vector)
        {
            buffer.AddRange(BitConverter.GetBytes(vector.x));
            buffer.AddRange(BitConverter.GetBytes(vector.y));
            buffer.AddRange(BitConverter.GetBytes(vector.z));
        }

        private void AddVector(VColor vector)
        {
            buffer.Add(vector.r);
            buffer.Add(vector.g);
            buffer.Add(vector.b);
            buffer.Add(vector.a);
        }
    }
}
