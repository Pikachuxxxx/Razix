﻿//Copyright © 2014 Sony Computer Entertainment America LLC. See License.txt.

using System;
using System.Drawing;
using System.IO;
using System.Reflection;
using System.Windows.Forms;

namespace Sce.Atf.Applications
{
    /// <summary>
    /// Enumeration for image sizes</summary>
    public enum ImageSizes
    {
        /// <summary>16 X 16 image</summary>
        e16x16 = 0,
        /// <summary>24 X 24 image</summary>
        e24x24,
        /// <summary>32 X 32 image</summary>
        e32x32
    }

    /// <summary>
    /// Class enabling xml-based skin configuration files to swap out registered Image data.
    /// Serves as a handle to a registered image (via ResourceUtil), and the id to which it was registered.
    /// One constructor will create, register and track the specified image, and dispose it on IDispose.Dispose().
    /// Another constructor simply tracks a pre-registered image (when the image to replace wasn't created/registered
    /// through some other library/assembly/API).  In this case, the image is not disposed.
    /// </summary>
    public class EmbeddedImage : IDisposable
    {
        /// <summary>
        /// Constructor which creates, registers, and tracks the specified image.
        /// We would have preferred to require an actual assembly, instead of its string name.
        /// However the purpose of this class, which is instantiated through XML data, prevents that.</summary>
        /// <param name="id">Image identifier</param>
        /// <param name="assemblyName">Assembly name</param>
        /// <param name="pathToImage">Full name of image resource</param>
        public EmbeddedImage(string id, string assemblyName, string pathToImage)
        {
            m_isOwner = true;
            m_id = id;
            var assembly = Assembly.LoadFrom(Application.StartupPath + Path.DirectorySeparatorChar + assemblyName);
            Image = GdiUtil.GetImage(assembly, pathToImage);
            ResourceUtil.RegisterImage(m_id, Image);
        }

        /// <summary>
        /// Constructor for simply tracking a pre-registered image.
        /// Because it was not created here, Image will not be disposed of in IDispose.Dispose().</summary>
        /// <param name="id">Image identifier</param>
        /// <param name="size">Image size</param>
        public EmbeddedImage(string id, ImageSizes size)
        {
            m_isOwner = false;
            m_id = id;
            switch (size)
            {
                case ImageSizes.e16x16: Image = ResourceUtil.GetImage16(id); break;
                case ImageSizes.e24x24: Image = ResourceUtil.GetImage24(id); break;
                case ImageSizes.e32x32: Image = ResourceUtil.GetImage32(id); break;

                default:
                    throw new ArgumentException("Invalid size specified");
            }

            if (Image == null)
                throw new ArgumentException("No image registered with this id and size");
        }

        /// <summary>
        /// Performs application-defined tasks associated with freeing, releasing, or
        /// resetting unmanaged resources</summary>
        public virtual void Dispose()
        {
            if (m_isOwner && Image != null)
                Image.Dispose();
        }

        /// <summary>
        /// Registers the image</summary>
        public void Reregister()
        {
            ResourceUtil.RegisterImage(m_id, Image);
        }

        /// <summary>
        /// Gets the image</summary>
        public Image Image { get; private set; }

        private readonly string m_id;
        private readonly bool m_isOwner;
    }
}
