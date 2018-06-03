////////////////////////////////////////////////////////////////////////////////////////
// CodePopupWindow.xaml.cs : This package provide the GUI for the client and its           //
// ver 1.0              functionalities                                               //
//                                                                                    //
//Language:      C++ 11                                                               //
// Platform    : HP Omen, Win Pro 10, Visual Studio 2017                              //
// Application : CSE-687 OOD Project 3                                                //
// Author      : Shashank Singh ,Syracuse University                                  //
// Source      : Dr. Jim Fawcett, EECS, SU                                            //
////////////////////////////////////////////////////////////////////////////////////////

/*
* Module Operations:
*===================
*This module provide the GUI for the client and its functionalities 
*
* Build Process:
* --------------
*   devenv Project4SSS.sln /debug rebuild
*
* public Interfaces:
* =================
* CodePopupWindow(): It intializes the content
*
* Maintainance History:
* =====================
* ver 1.0
*
*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace GUI
{
    /// <summary>
    /// Interaction logic for CodePopupWindow.xaml
    /// </summary>
    public partial class CodePopupWindow : Window
    {
        public CodePopupWindow()
        {
            InitializeComponent();
        }
        private void exitButton_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    }
}
