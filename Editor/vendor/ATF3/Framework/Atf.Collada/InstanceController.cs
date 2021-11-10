﻿//Copyright © 2014 Sony Computer Entertainment America LLC. See License.txt.

using System.Collections.Generic;

using Sce.Atf.Adaptation;
using Sce.Atf.Dom;
using Sce.Atf.Rendering;

namespace Sce.Atf.Collada
{
    /// <summary>
    /// COLLADA instance controller</summary>
    public class InstanceController : DomNodeAdapter, ISceneGraphHierarchy
    {
        /// <summary>
        /// Performs initialization when the adapter's node is set.
        /// This method is called each time the adapter is connected to its underlying node.
        /// Typically overridden by creators of DOM adapters.</summary>
        protected override void OnNodeSet()
        {
            base.OnNodeSet();

            DomNode controller = DomNode.GetAttribute(Schema.instance_controller.urlAttribute).As<DomNode>();
            DomNode skin = controller.GetChild(Schema.controller.skinChild);
            Geometry = skin.GetAttribute(Schema.skin.sourceAttribute).As<Geometry>();

            Geometry.Effects = Tools.CreateEffectDictionary(GetChild<DomNode>(Schema.instance_controller.bind_materialChild));
        }

        /// <summary>
        /// Gets or sets Geometry</summary>
        public Geometry Geometry { get; set; }

        #region ISceneGraphHierarchy Members

        /// <summary>
        /// Gets enumeration of the children of the object</summary>
        /// <returns>Enumeration of the children of the object</returns>
        public IEnumerable<object> GetChildren()
        {
            yield return Geometry.DomNode;
        }

        #endregion
    }
}
