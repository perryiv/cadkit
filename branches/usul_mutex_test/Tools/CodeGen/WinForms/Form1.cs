
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Microsoft.Win32;
using System.Xml;
using System.Xml.Xsl;

namespace CodeGenWF
{
    public partial class Form1 : Form
    {

        ///////////////////////////////////////////////////////////////////////////////
        //
        //  Constructor.
        //
        ///////////////////////////////////////////////////////////////////////////////

        public Form1()
        {
            InitializeComponent();

            this._populateInterfacesListBox();


            RegistryKey hkcu = Registry.CurrentUser;
            RegistryKey hkSoftware = hkcu.CreateSubKey( "Software" );
            RegistryKey hkCadkit = hkSoftware.CreateSubKey( "CadKit" );
            RegistryKey hkCodeGen = hkCadkit.CreateSubKey( "CodeGen" );
            _creator.Text = (string)hkCodeGen.GetValue( "Creator" );
            _pluginDirectory.Text = (string)hkCodeGen.GetValue( "PluginDirectory" );
            _interfaceDirectory.Text = (string)hkCodeGen.GetValue( "InterfaceDirectory" );
        }


        ///////////////////////////////////////////////////////////////////////////////
        //
        //  Populate the interface list box.
        //
        ///////////////////////////////////////////////////////////////////////////////

        private void _populateInterfacesListBox()
        {
            String usulDir = System.Environment.GetEnvironmentVariable( "CADKIT_ROOT_DIR" ) + "\\Usul\\Interfaces\\";

            _interfaces.ResetText();

            String[] files = System.IO.Directory.GetFiles( usulDir, "*.h" );

            for (int i = 0; i < files.Length; ++i)
            {
                _interfaces.Items.Add( files[i] );
            }
        }


        ///////////////////////////////////////////////////////////////////////////////
        //
        //  Close the form.
        //
        ///////////////////////////////////////////////////////////////////////////////

        private void _closeButton_Click( object sender, EventArgs e )
        {
            this.Close();
        }


        ///////////////////////////////////////////////////////////////////////////////
        //
        //  The form is closing.
        //
        ///////////////////////////////////////////////////////////////////////////////

        private void _formClosing( object sender, FormClosingEventArgs e )
        {
            RegistryKey hkcu = Registry.CurrentUser;
            RegistryKey hkSoftware = hkcu.CreateSubKey( "Software" );
            RegistryKey hkCadkit = hkSoftware.CreateSubKey( "CadKit" );
            RegistryKey hkCodeGen = hkCadkit.CreateSubKey( "CodeGen" );
            hkCodeGen.SetValue( "Creator", _creator.Text );
            hkCodeGen.SetValue( "PluginDirectory", _pluginDirectory.Text );
            hkCodeGen.SetValue( "InterfaceDirectory", _interfaceDirectory.Text );
        }


        ///////////////////////////////////////////////////////////////////////////////
        //
        //  Browse for the plugin directory.
        //
        ///////////////////////////////////////////////////////////////////////////////

        private void button1_Click( object sender, EventArgs e )
        {
            System.Windows.Forms.FolderBrowserDialog dialog = new System.Windows.Forms.FolderBrowserDialog();

            if (dialog.ShowDialog() == DialogResult.OK)
            {
                _pluginDirectory.Text = dialog.SelectedPath;
            }
        }


        ///////////////////////////////////////////////////////////////////////////////
        //
        //  Browse the interface directory
        //
        ///////////////////////////////////////////////////////////////////////////////

        private void button2_Click( object sender, EventArgs e )
        {
            System.Windows.Forms.FolderBrowserDialog dialog = new System.Windows.Forms.FolderBrowserDialog();

            if (dialog.ShowDialog() == DialogResult.OK)
            {
                _interfaceDirectory.Text = dialog.SelectedPath;
            }
        }


        ///////////////////////////////////////////////////////////////////////////////
        //
        //  Create the plugin.
        //
        ///////////////////////////////////////////////////////////////////////////////

        private void _pluginCreate_Click( object sender, EventArgs e )
        {
            this._createPluginFiles();
            _log.AppendText("Created plugin " + _pluginName.Text );
        }


        ///////////////////////////////////////////////////////////////////////////////
        //
        //  Create the interface.
        //
        ///////////////////////////////////////////////////////////////////////////////

        private void _createInterface_Click( object sender, EventArgs e )
        {
            this._createUsulInterface();
            _log.AppendText("Created interface " + _interfaceName.Text );
        }


        ///////////////////////////////////////////////////////////////////////////////
        //
        //  Create the plugin files.
        //
        ///////////////////////////////////////////////////////////////////////////////

        private void _createPluginFiles()
        {
            XmlDocument xml = _createXmlDocument();


            //XmlTextWriter wr = new XmlTextWriter("Text.xml", Encoding.UTF8);
            //wr.Formatting = Formatting.Indented;

            //xml.WriteContentTo(wr);
            //wr.Close();

            this._transform( xml, "CompileGuard.xsl", _pluginDirectory.Text + "\\CompileGuard.h" );
            this._transform( xml, "Export.xsl", _pluginDirectory.Text + "\\Export.def" );
            this._transform( xml, "Makefile.xsl", _pluginDirectory.Text + "\\Makefile" );

            String componentClassName = _componentClassName();

            this._transform( xml, "ComponentHeader.xsl", _pluginDirectory.Text + "\\" + componentClassName + ".h" );
            this._transform( xml, "ComponentSource.xsl", _pluginDirectory.Text + "\\" + componentClassName + ".cpp" );

            String factoryClassName = _factoryClassName();

            this._transform( xml, "FactoryHeader.xsl", _pluginDirectory.Text + "\\" + factoryClassName + ".h" );
            this._transform( xml, "FactorySource.xsl", _pluginDirectory.Text + "\\" + factoryClassName + ".cpp" );


            // Create Visual Studio project files.
            this._createProjectFiles();
        }


        ///////////////////////////////////////////////////////////////////////////////
        //
        //  Get the name of the factory class.
        //
        ///////////////////////////////////////////////////////////////////////////////

        private String _factoryClassName()
        {
            String factoryClassName = _pluginName.Text + "Factory";
            factoryClassName = factoryClassName.Replace( " ", "" );
            return factoryClassName;
        }


        ///////////////////////////////////////////////////////////////////////////////
        //
        //  Get the name of the component class.
        //
        ///////////////////////////////////////////////////////////////////////////////

        private String _componentClassName()
        {
            String componentClassName = _pluginName.Text + "Component";
            componentClassName = componentClassName.Replace( " ", "" );
            return componentClassName;
        }


        ///////////////////////////////////////////////////////////////////////////////
        //
        //  Transform the input xml into the output file.
        //
        ///////////////////////////////////////////////////////////////////////////////

        private void _transform( XmlDocument xml, String xlstfile, String outfile )
        {
            XslTransform xlst = new XslTransform();

            String xlstDir = _resourceFileDirectory();

            xlst.Load( xlstDir + xlstfile );

            System.Xml.Xsl.XsltArgumentList args = new System.Xml.Xsl.XsltArgumentList();

            System.IO.FileInfo info = new System.IO.FileInfo( outfile );
            
            System.IO.FileStream output;

            if ( info.Exists )
                output = new System.IO.FileStream( outfile, System.IO.FileMode.Truncate );
            else
                output = new System.IO.FileStream( outfile, System.IO.FileMode.Create );

            xlst.Transform( xml.CreateNavigator(), args, output );

            output.Close();
        }


        ///////////////////////////////////////////////////////////////////////////////
        //
        //  Create the xml document.
        //
        ///////////////////////////////////////////////////////////////////////////////

        XmlDocument _createXmlDocument()
        {
            System.Xml.XmlDocument xml = new System.Xml.XmlDocument();

            XmlElement pluginTemplate = xml.CreateElement( "PluginTemplate" );
            pluginTemplate.SetAttribute( "name", _pluginName.Text.Replace( " ", "" ) );

            // Append the user element
            pluginTemplate.AppendChild( this._createUserElement( xml ) );

            // Append the compile guard element.
            pluginTemplate.AppendChild( this._createCompileGuardElement( xml ) );

            // Append the Component Source Element.
            pluginTemplate.AppendChild( this._createComponentSourceElement( xml ) );

            // Append the Component Header Element.
            pluginTemplate.AppendChild( this._createComponentHeaderElement( xml ) );

            // Append the Factory Source element.
            pluginTemplate.AppendChild( this._createFactorySourceElement( xml ) );

            // Append the Factory Header element.
            pluginTemplate.AppendChild( this._createFactoryHeaderElement( xml ) );

            // Append the plugin template element to the document.
            xml.AppendChild( pluginTemplate );

            return xml;
        }


        ///////////////////////////////////////////////////////////////////////////////
        //
        //  Create the xml element for the user.
        //
        ///////////////////////////////////////////////////////////////////////////////

        XmlElement _createUserElement( XmlDocument xml )
        {
            XmlElement user = xml.CreateElement( "User" );
            user.SetAttribute( "name", _creator.Text );

            return user;
        }


        ///////////////////////////////////////////////////////////////////////////////
        //
        //  Create the xml element for the compile guard.
        //
        ///////////////////////////////////////////////////////////////////////////////

        XmlElement _createCompileGuardElement( XmlDocument xml )
        {
            String headerDefine = _pluginName.Text.ToUpper();

            headerDefine.Replace( ' ', '_' );
            String compileGuard = "_COMPILING_" + headerDefine;

            headerDefine = "__" + headerDefine + "_COMPILE_GUARD_H__";

            XmlElement compileGuardElement = xml.CreateElement( "CompileGuard" );
            compileGuardElement.SetAttribute( "headerDefine", headerDefine );
            compileGuardElement.SetAttribute("compilePreprocess", compileGuard);

            return compileGuardElement;
        }


        ///////////////////////////////////////////////////////////////////////////////
        //
        //  Create the xml element for the component source.
        //
        ///////////////////////////////////////////////////////////////////////////////

        XmlElement _createComponentSourceElement( XmlDocument xml )
        {
            XmlElement element = xml.CreateElement( "ComponentSource" );
            return element;
        }


        ///////////////////////////////////////////////////////////////////////////////
        //
        //  Create the xml element for the component header.
        //
        ///////////////////////////////////////////////////////////////////////////////

        XmlElement _createComponentHeaderElement( XmlDocument xml )
        {
            XmlElement element = xml.CreateElement( "ComponentHeader" );

            // Make the component class name.
            String componentClassName = this._componentClassName();

            String includeGuard = "__" + componentClassName + "_H__";
            includeGuard = includeGuard.ToUpper();

            element.SetAttribute( "classDescription", "" );
            element.SetAttribute( "compileGuard", includeGuard );
            element.SetAttribute( "className", componentClassName );

            //Add the include files from Usul/Interfaces.
            for (int i = 0; i < _interfaces.SelectedItems.Count; ++i)
            {
                String file = _interfaces.SelectedItems[i].ToString();
                int dot = file.IndexOf( "." );
                int slash = file.LastIndexOf( "\\" );
                slash++;
                String usulInterface = file.Substring( slash, dot - slash );

                XmlElement e = xml.CreateElement( "UsulInterface" );
                e.SetAttribute( "name", usulInterface );

                element.AppendChild( e );
            }

            return element;
        }


        ///////////////////////////////////////////////////////////////////////////////
        //
        //  Create the xml element for the factory source.
        //
        ///////////////////////////////////////////////////////////////////////////////

        XmlElement _createFactorySourceElement( XmlDocument xml )
        {
            XmlElement element = xml.CreateElement( "FactorySource" );

            XmlElement e = xml.CreateElement( "LibInclude" );
            String fileName = _pluginName.Text.Replace( " ", "" ) + "Component.h";
            e.SetAttribute( "file", fileName );

            element.AppendChild( e );

            return element;
        }


        ///////////////////////////////////////////////////////////////////////////////
        //
        //  Create the xml element for the factory header.
        //
        ///////////////////////////////////////////////////////////////////////////////

        XmlElement _createFactoryHeaderElement( XmlDocument xml )
        {
            XmlElement element = xml.CreateElement( "FactoryHeader" );
            
            String factoryClassName = this._factoryClassName();

            String includeGuard = "__" + factoryClassName + "_H__";
            includeGuard = includeGuard.ToUpper();

            element.SetAttribute( "classDescription", "" );
            element.SetAttribute( "compileGuard", includeGuard );
            element.SetAttribute( "className", factoryClassName );

            return element;
        }


        ///////////////////////////////////////////////////////////////////////////////
        //
        //  Create the xml element for the Usul interface.
        //
        ///////////////////////////////////////////////////////////////////////////////

        XmlElement _createUsulInterfacesElement( XmlDocument xml )
        {
            XmlElement element = xml.CreateElement( "UsulHeader" );

            String s = DateTime.Now.Ticks.ToString();
            long l = long.Parse( s );
            UInt32 i = (UInt32) ( l & 0x0FFFFFFF );
            String iid = "" + i;

            String guard = _interfaceName.Text + "_H__";
            guard = guard.ToUpper();

            element.SetAttribute( "classDescription", "" );
            element.SetAttribute( "compileGuard", guard );
            element.SetAttribute( "className", _interfaceName.Text );
            element.SetAttribute( "IID", iid );
            
            return element;
        }


        ///////////////////////////////////////////////////////////////////////////////
        //
        //  Create the project files..
        //
        ///////////////////////////////////////////////////////////////////////////////

        void _createProjectFiles()
        {
            String dir = _resourceFileDirectory();

            String pluginName = _pluginName.Text.Replace( " ", "" );

            {
                String outfile = _pluginDirectory.Text + "\\" + pluginName + ".vcproj";
                String infile = dir + "skeleton_vs71.xml";
                _writeVisualStudioFile( outfile, infile );
            }

            {
                String outfile = _pluginDirectory.Text + "\\" + pluginName + "_vs8.vcproj";
                String infile = dir + "skeleton_vs80.xml";
                _writeVisualStudioFile( outfile, infile );
            }
        }


        ///////////////////////////////////////////////////////////////////////////////
        //
        //  Write the visual studio file.
        //
        ///////////////////////////////////////////////////////////////////////////////

        private void _writeVisualStudioFile( String outfile, String infile )
        {
            String plugin = _pluginName.Text;

            String pluginName = _pluginName.Text.Replace( " ", "" );

            String compileGuard = _pluginName.Text.ToUpper().Replace( ' ', '_' );

            // Make a guid.
            String guid = Guid.NewGuid().ToString();

            String component = this._componentClassName();
            String factory = this._factoryClassName();

            System.IO.StreamReader stream = new System.IO.StreamReader( infile );

            String contents = stream.ReadToEnd();

            contents = contents.Replace( "CM_PLUGIN_TITLE", plugin );
            contents = contents.Replace( "CM_PLUGIN_NAME", pluginName );
            contents = contents.Replace( "CM_GUID", guid );
            contents = contents.Replace( "CM_COMPILE_GUARD", compileGuard );
            contents = contents.Replace( "CM_COMPONENT_NAME", component );
            contents = contents.Replace( "CM_FACTORY_NAME", factory );

            System.IO.StreamWriter writer = new System.IO.StreamWriter( outfile );
            writer.Write( contents );
            writer.Close();
        }


        ///////////////////////////////////////////////////////////////////////////////
        //
        //  Get the directory that contains the resource files.
        //
        ///////////////////////////////////////////////////////////////////////////////

        private static String _resourceFileDirectory()
        {
            String dir = System.Environment.GetEnvironmentVariable( "CADKIT_ROOT_DIR" ) + "\\Tools\\CodeGen\\XLST\\";
            return dir;
        }


        ///////////////////////////////////////////////////////////////////////////////
        //
        //  Create the interface.
        //
        ///////////////////////////////////////////////////////////////////////////////

        private void _createUsulInterface()
        {
            String dir = System.Environment.GetEnvironmentVariable("CADKIT_ROOT_DIR") + "\\Usul\\Interfaces\\";

            System.Xml.XmlDocument xml = new System.Xml.XmlDocument();

            XmlElement pluginTemplate = xml.CreateElement( "PluginTemplate" );
            
            // Append the user element
            pluginTemplate.AppendChild( this._createUserElement( xml ) );

            // Append the Usul Header element.
            pluginTemplate.AppendChild( this._createUsulInterfacesElement( xml ) );

            // Append the plugin template element to the document.
            xml.AppendChild( pluginTemplate );

            this._transform( xml, "UsulHeader.xsl", _interfaceDirectory.Text + "\\" + _interfaceName.Text + ".h" );
            
            this._populateInterfacesListBox();
        }
    }
}