﻿//Copyright © 2014 Sony Computer Entertainment America LLC. See License.txt.

using System.ComponentModel;

using Sce.Atf.Wpf.Docking;

namespace Sce.Atf.Wpf.Applications
{
    /// <summary>
    /// Class that holds information about controls hosted by IControlHostService
    /// implementations.</summary>
    public class ControlInfo : IControlInfo
    {
        /// <summary>
        /// Constructor</summary>
        /// <param name="id">Unique ID for the control</param>
        /// <param name="group">Initial control group for the control</param>
        /// <param name="dockContent">The IDockContent representing the control</param>
        /// <param name="client">The client that registered this control</param>
        public ControlInfo(string id, Sce.Atf.Applications.StandardControlGroup group, IDockContent dockContent, IControlHostClient client)
            : this(null, null, id, group, null, dockContent, client)
        {
        }

        /// <summary>
        /// Constructor</summary>
        /// <param name="name">Control's name, which may be displayed as the title of 
        /// the hosting control or form</param>
        /// <param name="description">Control's description, which may be displayed as 
        /// a tooltop when hovering over the control</param>
        /// <param name="id">Unique ID for the control</param>
        /// <param name="group">Initial control group for the control</param>
        /// <param name="dockContent">The IDockContent representing the control</param>
        /// <param name="client">The client that registered this control</param>
        public ControlInfo(string name, string description, string id, Sce.Atf.Applications.StandardControlGroup group, IDockContent dockContent, IControlHostClient client)
            : this(name, description, id, group, null, dockContent, client)
        {
        }

        /// <summary>
        /// Constructor</summary>
        /// <param name="name">Control's name, which may be displayed as the title of 
        /// the hosting control or form</param>
        /// <param name="description">Control's description, which may be displayed as 
        /// a tooltop when hovering over the control</param>
        /// <param name="id">Unique ID for the control</param>
        /// <param name="group">Initial control group for the control</param>
        /// <param name="imageKey">The control's image, which may be displayed on the 
        /// hosting control or form</param>
        /// <param name="dockContent">The IDockContent representing the control</param>
        /// <param name="client">The client that registered this control</param>
        public ControlInfo(string name, string description, string id, Sce.Atf.Applications.StandardControlGroup group, object imageKey, IDockContent dockContent, IControlHostClient client)
        {
            Requires.NotNullOrEmpty(id, "id");
            Requires.NotNull(dockContent, "dockContent");
            Requires.NotNull(client, "client");

            DockContent = dockContent;
            dockContent.PropertyChanged += DockContent_PropertyChanged;
            Name = name;
            Description = description;
            Id = id;
            Group = group;
            ImageSourceKey = imageKey;
            Client = client;
        }

        #region IControlInfo Members

        /// <summary>
        /// Gets or sets the control's name, which may be displayed as the title of
        /// a hosting control or form</summary>
        public string Name
        {
            get { return DockContent.Header; }
            set 
            { 
                DockContent.Header = value;
                if (m_command != null)
                {
                    m_command.Text = value;
                    m_command.Description = "Show/Hide ".Localize() + value;
                }
            }
        }

        /// <summary>
        /// Gets or sets the control's description, which may be displayed as a tooltip
        /// when the user hovers over a hosting control or form</summary>
        public string Description
        {
            get { return m_description; }
            set {  m_description = value; }
        }

        /// <summary>
        /// Gets or sets the control's image, which may be displayed on a hosting control
        /// or form</summary>
        public object ImageSourceKey
        {
            get { return m_imageKey; }
            set
            {
                m_imageKey = value;
                DockContent.Icon = m_imageKey;
            }
        }

        /// <summary>
        /// Gets unique ID for control</summary>
        public string Id { get; private set; }

        /// <summary>
        /// Gets the desired initial control group</summary>
        public Sce.Atf.Applications.StandardControlGroup Group { get; private set; }

        /// <summary>
        /// Gets the client that registered this control</summary>
        public IControlHostClient Client { get; private set; }

        /// <summary>
        /// Gets the control that was registered with this info</summary>
        public object Content { get { return DockContent.Content; } }

        #endregion

        /// <summary>
        /// Gets the DockContent representing the control</summary>
        public IDockContent DockContent { get; private set; }
        
        /// <summary>
        /// Gets and sets the menu/toolbar command for the control</summary>
        public ICommandItem Command 
        {
            get { return m_command; }
            set
            {
                m_command = value;
                if (m_command != null)
                {
                    m_command.IsChecked = DockContent.IsVisible;
                    m_command.Text = Name;
                    m_command.Description = "Show/Hide ".Localize() + Name;
                }
            }
        }

        private void DockContent_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (Command != null && e.PropertyName == s_isVisiblePropertyName)
            {
                Command.IsChecked = ((IDockContent)sender).IsVisible;
            }
        }

        private string m_description;
        private object m_imageKey;
        private ICommandItem m_command;
        private static readonly string s_isVisiblePropertyName
            = TypeUtil.GetProperty<IDockContent>(x => x.IsVisible).Name;
    }
}
