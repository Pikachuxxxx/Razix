//Copyright © 2014 Sony Computer Entertainment America LLC. See License.txt.

using System.ComponentModel.Composition;
using System.IO;
using System.Reflection;
using System.Windows.Forms;
using Sce.Atf;
using Sce.Atf.Controls;
using System.Collections.Generic;   // for IList

namespace Razix
{
    /// <summary>
    /// Adds the Help/About command, which displays a dialog box with a description
    /// of the application (specified by a derived class) plus the ATF version number</summary>
    [Export(typeof(IInitializable))]
    [Export(typeof(HelpAboutCommand))]
    [PartCreationPolicy(CreationPolicy.Shared)]
    public class HelpAboutCommand : Sce.Atf.Applications.HelpAboutCommand
    {
        /// <summary>
        /// Shows the About dialog box</summary>
        /// <remarks>Derived classes should copy this method and customize it.
        /// It is recommended to display the ATF version number to aid in diagnosing
        /// problems. (Pass in 'true' to AboutDialog's constructor or use AtfVersion.)</remarks>
        protected override void ShowHelpAbout()
        {
            RichTextBox richTextBox = new RichTextBox();
            richTextBox.BorderStyle = BorderStyle.None;
            richTextBox.ReadOnly = true;
            Stream textFileStream = Assembly.GetExecutingAssembly().GetManifestResourceStream(
                "Razix.Resources.AboutRazixEditor.rtf");
            richTextBox.LoadFile(textFileStream, RichTextBoxStreamType.RichText);

            string appURL = "https://github.com/Pikachuxxxx/RazixCodeEditor";

            // logo, crtedits, addinfo
            IList<string> strings = new List<string> { "Razix Engine by Pikachuxxxx aka Phani Srikar \n\n" };

            AboutDialog dialog = new AboutDialog(
                "Razix Code Editor".Localize(), appURL, richTextBox, GdiUtil.GetImage("Razix.Resources.RazixAtf.png"), strings, true);
            dialog.ShowDialog();
        }
    }
}
