//Copyright © 2014 Sony Computer Entertainment America LLC. See License.txt.

#define PERFORCE_VERSION_CONTROL // Uses Perforce by default. Uncomment to use Subversion.

using System;
using System.ComponentModel.Composition;
using System.ComponentModel.Composition.Hosting;
using System.Threading;
using System.Windows.Forms;

// SCE Sony ATF vendor
using Sce.Atf;
using Sce.Atf.Applications;

namespace Razix
{
     class EditorMain
    {
        /// <summary>
        /// The main entry point for the Editor application
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            // important to call these before creating application host
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.DoEvents(); // see http://www.codeproject.com/buglist/EnableVisualStylesBug.asp?df=100&forumid=25268&exp=0&select=984714

            // Set up localization support early on, so that user-readable strings will be localized
            //  during the initialization phase below. Use XML files that are embedded resources.
            Thread.CurrentThread.CurrentUICulture = System.Globalization.CultureInfo.CurrentCulture;
            Localizer.SetStringLocalizer(new EmbeddedResourceStringLocalizer());

            var catalog = new TypeCatalog(
                typeof(SettingsService),                // persistent settings and user preferences dialog
                typeof(StatusService),                  // status bar at bottom of main Form
                typeof(CommandService),                 // handles commands in menus and toolbars
                typeof(ControlHostService),             // docking control host
                typeof(WindowLayoutService),            // multiple window layout support
                typeof(WindowLayoutServiceCommands),    // window layout commands
                typeof(FileDialogService),              // standard Windows file dialogs
                typeof(AutoDocumentService),            // opens documents from last session, or creates a new document, on startup
                typeof(Outputs),                        // service that provides static methods for writing to IOutputWriter objects.
                typeof(OutputService),                  // rich text box for displaying error and warning messages. Implements IOutputWriter.
                typeof(RecentDocumentCommands),         // standard recent document commands in File menu
                typeof(StandardFileCommands),           // standard File menu commands for New, Open, Save, SaveAs, Close
                typeof(StandardFileExitCommand),        // standard File exit menu command
                typeof(AtfUsageLogger),                 // logs computer info to an ATF server
                typeof(CrashLogger),                    // logs unhandled exceptions to an ATF server
                typeof(UnhandledExceptionService),      // catches unhandled exceptions, displays info, and gives user a chance to save
                typeof(ContextRegistry),                // central context registry with change notification
                typeof(DocumentRegistry),               // central document registry with change notification
                typeof(MainWindowTitleService),         // tracks document changes and updates main form title
                typeof(TabbedControlSelector),          // enable ctrl-tab selection of documents and controls within the app
                typeof(DefaultTabCommands),             // provides the default commands related to document tab Controls
                typeof(SkinService),

                // Customized components of the base class
                typeof(HelpAboutCommand)                // Help -> About command with custom information
            );

            var container = new CompositionContainer(catalog);

            var toolStripContainer = new ToolStripContainer();
            toolStripContainer.Dock = DockStyle.Fill;

            var mainForm = new MainForm(toolStripContainer);
            // TODO: Add Razix Editor logo later
            //var image = GdiUtil.GetImage("Razix.Resources.RazixCodeEditorLogo.ico");
            //mainForm.Icon = GdiUtil.CreateIcon(image, 32, true);

            mainForm.Text = "Razix Editor".Localize();

            var batch = new CompositionBatch();
            batch.AddPart(mainForm);
            //batch.AddPart(new WebHelpCommands("https://github.com/SonyWWS/ATF/wiki/ATF-Code-Editor-Sample".Localize()));
            container.Compose(batch);

            // To make the tab commands (e.g., "Copy Full Path", "Open Containing Folder") available, we have to change
            //  the default behavior to work with this sample app's unusual Editor. In most cases, an editor like this
            //  would implement IDocumentClient and this customization of DefaultTabCommands wouldn't be necessary.
            var tabCommands = container.GetExportedValue<DefaultTabCommands>();

#if !PERFORCE_VERSION_CONTROL
            var sourceControlCommands = container.GetExportedValue<SourceControlCommands>();
            sourceControlCommands.RefreshStatusOnSave = true;
#endif

            // Initialize components that require it. Initialization often can't be done in the constructor,
            //  or even after imports have been satisfied by MEF, since we allow circular dependencies between
            //  components, via the System.Lazy class. IInitializable allows components to defer some operations
            //  until all MEF composition has been completed.
            container.InitializeAll();
            
            // Show the main form and start message handling. The main Form Load event provides a final chance
            //  for components to perform initialization and configuration.
            Application.Run(mainForm);

            container.Dispose();
        }
    }
}
