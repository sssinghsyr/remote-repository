////////////////////////////////////////////////////////////////////////////////////////
// MainWindow.xaml.cs : This package provide the GUI for the client and its           //
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
*   devenv Project3SSS.sln /debug rebuild
*
* public Interfaces:
* =================
* MainWindow(): It intializes the content
* processMessages(): process incoming messages on child thread
* clearDirsCheckIn(): clears the list box for directories in check in tab
* clearDirsCheckOut(): clears the list box for directories in check out tab
* clearDirsBrowse(): clears the list box for directories in browse tab
* addDirCheckIn(): adds directory name to the list box for directories in checkin tab
* addDirCheckOut(): adds directory name to the list box for directories in checkout tab
* addDirBrowse(): adds directory name to the list box for directories in browse tab
* insertParentCheckIn(): insert arent directory to the directory listbox for checkin tab
* insertParentCheckOut(): insert arent directory to the directory listbox for checkout tab
* insertParentBrowse(): insert arent directory to the directory listbox for browse tab
* clearFilesCheckIn(): clear files listbox in checkIn tab
* clearFilesCheckOut(): clear files listbox in checkOut tab
* clearFilesBrowse(): clear files listbox in browse tab
* addFilesCheckIn(): add fileName to files list box in checkIn tab
* addFilesCheckOut(): add fileName to files list box in checkOut tab
* addFilesBrowse(): add fileName to files list box in browse tab
* addClientProc(): add client processing for message with key
* DispatcherLoadGetDirs(): dispatcher to load directories in GUI
* DispatcherLoadGetFiles(): dispatcher to load files in GUI
* Browse_MouseLeftButtonDown(): dispatcher to process connect reply from server
* SendButton_Click(): dispatcher to process checkIn reply from server
* DisconnectButton_Click(): dispatcher to process checkOut reply from server
* FileList_MouseDoubleClick(): dispatcher to process browseFiles reply from server
* Check_In_MouseLeftButtonDown(): dispatcher to process metaData reply from server
* CheckInButton_Click(): load dispatcher for all the commands
* AddDepCheck_Click(): handler to perform connection establishment
* AddCategory_Click(): handler for check-in button
* CIDirList_MouseDoubleClick(): handler for check-out button
* CISelectedFile_Checked(): handler for meta-data button
* addDependentKey(): handler for double click on directory item in browse tab
* rmDependentKey(): handler for double click on directory item in check-in tab
* CheckInFile_Click(): handler for double click on directory item in check-out tab
* test1():test stub to demonstrate requirement
* test2a():test stub to demonstrate requirement
* testConnection():test stub to demonstrate requirement
* testCheckIn():test stub to demonstrate requirement
* testCheckOut():test stub to demonstrate requirement
* testBrowsing():test stub to demonstrate requirement
* testfileView():test stub to demonstrate requirement
* testFileMetaData():test stub to demonstrate requirement
* files_browse_MouseDoubleClick(): handler for double click on files item in browse tab
* CheckOutButton_Click(): initially loads files and directories when checkin tab is selected
* Check_Out_MouseLeftButtonDown(): initially loads files and directories when checkout tab is selected
* View_MetaData_Click(): initially loads files and directories when browse tab is selected
* ViewMetaData_MouseLeftButtonDown(): Executed when the GUI window is loaded
* Required Files:
* ===============
* MainWindow.xaml, CsMessage.h, Translater.h
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
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Threading;
using System.Collections.ObjectModel;
using MsgPassingCommunication;

namespace GUI
{

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public class BoolStringClass
        {
            public string TheText { get; set; }
            public bool IsSelected { get; set; }
            public bool InList { get; set; }
        }
        public ObservableCollection<BoolStringClass> TheList { get; set; }
        public MainWindow()
        {
            TheList = new ObservableCollection<BoolStringClass>();
            serverEndPoint_ = new CsEndPoint();
            InitializeComponent();
            this.DataContext = this;
        }

        private Stack<string> pathStack_ = new Stack<string>();
        private Translater translater;
        private CsEndPoint endPoint_;
        private Thread rcvThrd = null;
        private Dictionary<string, Action<CsMessage>> dispatcher_
          = new Dictionary<string, Action<CsMessage>>();
        private string saveFilesPath = "../../../../SendFiles";
        private string sendFilesPath = "../../../../SaveFiles";
        private bool connection_flag = false;
        private bool test = true;
        private bool queryWin = false;
        private bool testQuery = true;
        private char currentWindow = 'b';
        public ObservableCollection<string> Items { get; } = new ObservableCollection<string>();
        private CsEndPoint serverEndPoint_ = null;
        private string connectedMsg = "Not Connected..";

        //----< process incoming messages on child thread >----------------

        private void processMessages()
        {
            ThreadStart thrdProc = () => {
                while (true)
                {
                    CsMessage msg = translater.getMessage();
                    string msgId = msg.value("command");
                    if (msgId == "")
                        continue;
                    if (dispatcher_.ContainsKey(msgId))
                        dispatcher_[msgId].Invoke(msg);
                }
            };
            rcvThrd = new Thread(thrdProc);
            rcvThrd.IsBackground = true;
            rcvThrd.Start();
        }
        //----< function dispatched by child thread to main thread >-------

        private void clearDirs()
        {
            if (currentWindow == 'b')
                DirList.Items.Clear();
            else
                CIDirList.Items.Clear();
        }
        //----< function dispatched by child thread to main thread >-------

        private void addDir(string dir)
        {
            if (currentWindow == 'b')
                DirList.Items.Add(dir);
            else
                CIDirList.Items.Add(dir);
        }
        //----< function dispatched by child thread to main thread >-------

        private void insertParent()
        {
            if (currentWindow == 'b')
                DirList.Items.Insert(0, "..");
            else
                CIDirList.Items.Insert(0, "..");
        }
        //----< function dispatched by child thread to main thread >-------

        private void clearFiles()
        {
            if (currentWindow == 'b')
                FileList.Items.Clear();
            else
                TheList.Clear();
        }
        //----< function dispatched by child thread to main thread >-------

        private void addFile(string file)
        {
            if (currentWindow == 'b')
                FileList.Items.Add(file);
            else
            {
                string key = pathStack_.Peek().Split('/').LastOrDefault() + "::" + file;
                if (DependentFilesList.Items.Contains(key))
                    TheList.Add(new BoolStringClass { IsSelected = true, TheText = file, InList = true });
                else
                    TheList.Add(new BoolStringClass { IsSelected = false, TheText = file, InList = false });
            }
        }
        //----< add client processing for message with key >---------------

        private void addClientProc(string key, Action<CsMessage> clientProc)
        {
            dispatcher_[key] = clientProc;
        }

        //----< Show recv message >----------------

        private void ShowRcvMess(string name) {
            Console.WriteLine("\n----------------------------------------------------");
            Console.WriteLine("\nSERVER REPLY MESSAGE -> Message Recieved in "+name);
        }

        //----< load getDirs processing into dispatcher dictionary >-------

        private void DispatcherLoadGetDirs()
        {
            Action<CsMessage> getDirs = (CsMessage rcvMsg) =>
            {
                if (test)
                    ShowRcvMess("getDirectories");
                Action clrDirs = () =>
                {
                    clearDirs();
                };
                Dispatcher.Invoke(clrDirs, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext()) {
                    string key = enumer.Current.Key;
                    if (key.Contains("dir"))
                        LoadGetDirsDir(enumer.Current.Value);
                }
                Action insertUp = () =>
                {
                    insertParent();
                };
                Dispatcher.Invoke(insertUp, new Object[] { });
            };
            addClientProc("getDirs", getDirs);
        }

        //----< Load incoming dir lists >----------------

        private void LoadGetDirsDir(string value) {
            Action<string> doDir = (string dir) =>
            {
                if (test)
                    Console.WriteLine("getDirs - Directory: " + dir);
                addDir(dir);
            };
            Dispatcher.Invoke(doDir, new Object[] { value });
        }

        //----< show file Clients >----------------

        private void showFileCLI(string filename)
        {
            string text = System.IO.File.ReadAllText(filename);
            Console.WriteLine(text);
        }
        //----< load Query >------

        private void DispatcherLoadQuery()
        {
            Action<CsMessage> query = (CsMessage rcvMsg) =>
            {
                if (test)
                    ShowRcvMess("Query");
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext()){
                    string key = enumer.Current.Key;
                    if (key.Contains("echoMessage"))
                        Console.WriteLine(enumer.Current.Value);
                }
                Action displayQuery = () =>
                {
                    string szFileName = rcvMsg.value("sendingFile");
                    if (System.IO.File.Exists("../../../GUI/temp" + "/" + szFileName))
                        System.IO.File.Delete("../../../GUI/temp" + "/" + szFileName);
                    Thread.Sleep(1000);
                    System.IO.File.Move(saveFilesPath + "/" + szFileName,
                                        "../../../GUI/temp" + "/" + szFileName);
                    if(testQuery)
                        showFileCLI("../../../GUI/temp" + "/" + szFileName);
                    else
                        showFile("../../../GUI/temp" + "/" + szFileName, queryWin ? "Query Window" : "DataBase Window", queryWin ?  "Query Result" : "ShowDb Result");
                    CleanQueryEntries();
                    System.IO.File.Delete("../../../GUI/temp" + "/" + szFileName);
                };
                Dispatcher.Invoke(displayQuery, new Object[] { });
            };
            addClientProc("query", query);
        }

        //----< Clear Window >----------------

        private void CleanQueryEntries() {
            this.QFileName.Text = "";
            this.QNameSpace.Text = "";
            this.QCatgs.Text = "";
            this.QDeps.Text = "";
            this.QDescrip.Text = "";
            this.Qfrom.Text = "From";
            this.Qto.Text = "To";
            this.closeStatusC.IsChecked = true;
            this.closeStatusO.IsChecked = true;
        }

        //----< load getFiles processing into dispatcher dictionary >------

        private void DispatcherLoadRcvFile()
        {
            Action<CsMessage> rcvFile = (CsMessage rcvMsg) =>
            {
                if (test)
                    ShowRcvMess("RecieveFile");
                Action displayFile = () =>
                {
                    string szFileName = rcvMsg.value("sendingFile");
                    if (currentWindow != 'o'){
                        //string szFileContent = System.IO.File.ReadAllText(saveFilesPath + "/" + szFileName);
                        showFile(saveFilesPath + "\\" + szFileName,"File View Window", szFileName);
                    }
                    else {
                        string ordName = szFileName.Substring(0, szFileName.Length - 2);
                        if (System.IO.File.Exists("../../../CHECK_OUT_FILES" + "/" + ordName))
                            System.IO.File.Delete("../../../CHECK_OUT_FILES" + "/" + ordName);
                        System.IO.File.Move(saveFilesPath + "/" + szFileName,
                                            "../../../CHECK_OUT_FILES" + "/" + ordName);
                        this.statusBarText.Text = "File checkout at ../CHECK_OUT_FILES" + "/" + ordName;
                        if(test)
                            Console.WriteLine("File recieved :["+ "../../../CHECK_OUT_FILES" + "/" + ordName + "]");
                    }
                };
                Dispatcher.Invoke(displayFile, new Object[] { });

                Console.WriteLine("File sent message recieved");
            };
            addClientProc("sendFile", rcvFile);
        }
        //----< load getFiles processing into dispatcher dictionary >------

        private void DispatcherLoadGetFile()
        {
            Action<CsMessage> getFile = (CsMessage rcvMsg) =>
            {
                if (test)
                    ShowRcvMess("getFile");
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("error"))
                    {
                        Action<string> mess = (string value) =>
                        {
                            this.statusBarText.Text = enumer.Current.Value;
                        };
                        Dispatcher.Invoke(mess, new Object[] { enumer.Current.Value });
                    }
                }
            };
            addClientProc("getFile", getFile);
        }

        //----< show File content in FileReadWin GUI from message getFile >----------------

        private void showFile(string filePath, string title, string name)
        {
            Paragraph paragraph = new Paragraph();
            //string fileSpec = saveFilesPath + "\\" + fileName;
            string fileText = System.IO.File.ReadAllText(filePath);
            paragraph.Inlines.Add(new Run(fileText));
            CodePopupWindow popUp = new CodePopupWindow();
            popUp.Title = title;
            popUp.codeLabel.Text = name;
            popUp.codeView.Blocks.Clear();
            popUp.codeView.Blocks.Add(paragraph);
            popUp.Show();
        }

        //----< load getFiles processing into dispatcher dictionary >------

        private void DispatcherLoadGetFiles()
        {
            Action<CsMessage> getFiles = (CsMessage rcvMsg) =>
            {
                if (test)
                    ShowRcvMess("getFiles");
                Action clrFiles = () =>
                {
                    clearFiles();
                };
                Dispatcher.Invoke(clrFiles, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("file"))
                    {
                        Action<string> doFile = (string file) =>
                        {
                            if (test)
                            {
                                Console.WriteLine("getFiles - File: "+file);
                            }
                            addFile(file);
                        };
                        Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
                    }
                }
            };
            addClientProc("getFiles", getFiles);
        }

        //----< load echo message processing into dispatcher dictionary >------

        private void DispatcherEcho()
        {
            if(test)
              ShowRcvMess("Echo");
            Action<CsMessage> echo = (CsMessage rcvMsg) =>
            {
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("message"))
                    {
                        Action<string> mess = (string value) =>
                        {
                            if (test)
                                Console.WriteLine("\nSERVER REPLY MESSAGE -> echo Message Recieved = "+ enumer.Current.Value);
                            this.ReceivedMsg.Text = enumer.Current.Value;
                        };
                        Dispatcher.Invoke(mess, new Object[] { enumer.Current.Value });

                    }
                }
            };
            addClientProc("echo", echo);
        }

        //----< load make new connection command processing into dispatcher dictionary >------

        private void DispatcherLoadMakeConnection()
        {
            Action<CsMessage> connection = (CsMessage rcvMsg) =>
            {
                if(test) ShowRcvMess("Connection");
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("message"))
                    {
                        Action<string> connect = (string status) =>
                        {
                            if (status == "Connected")
                            {
                                connection_flag = true;
                                connectedMsg = "Connected..";
                                this.statusBarText.Text = connectedMsg;
                                if(test)
                                    Console.WriteLine("\nSERVER REPLY MESSAGE -> Connected with Server");
                            }
                        };
                        Dispatcher.Invoke(connect, new Object[] { enumer.Current.Value });
                    }
                }
            };
            addClientProc("connection", connection);
        }

        //----< load local check-in file processing into dispatcher dictionary >------

        private void DispatcherCheckIn()
        {
            Action<CsMessage> checkIn = (CsMessage rcvMsg) =>
            {
                if(test) ShowRcvMess("CheckIn");
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext()){
                    string key = enumer.Current.Key;
                    if (key.Contains("return")){
                        Action<string> mess = (string value) =>
                        {
                            this.statusBarText.Text = enumer.Current.Value;
                            CleanCheckInWinEntries();
                        };
                        Dispatcher.Invoke(mess, new Object[] { enumer.Current.Value });
                    }
                }
            };
            addClientProc("checkIn", checkIn);
        }

        //----< load check-out server file into local machine processing into dispatcher dictionary >------

        private void DispatcherCheckOut()
        {
            Action<CsMessage> checkOut = (CsMessage rcvMsg) =>
            {
                if(test) ShowRcvMess("CheckOut");
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("error"))
                    {
                        Action<string> mess = (string value) =>
                        {
                            this.statusBarText.Text = enumer.Current.Value;
                        };
                        Dispatcher.Invoke(mess, new Object[] { enumer.Current.Value });
                    }
                }
            };
            addClientProc("checkOut", checkOut);
        }

        //----< load check-out server file into local machine processing into dispatcher dictionary >------

        private void DispatcherNoParent()
        {
            Action<CsMessage> noParent = (CsMessage rcvMsg) =>
            {
                if(test) ShowRcvMess("ShowNoParent");
                Console.WriteLine("Demostration for Requirement #3c");
                var enumer = rcvMsg.attributes.GetEnumerator();
                if (test)
                    Console.WriteLine("\nFollowing are the files with NO PARENTS");
                while (enumer.MoveNext()){
                    string key = enumer.Current.Key;
                    if (key.Contains("key")){
                        Action<string> mess = (string value) =>
                        {
                            if(test)
                                Console.WriteLine(value);
                            this.NoParentResult.Items.Add(value);
                        };
                        Dispatcher.Invoke(mess, new Object[] { enumer.Current.Value });
                    }
                }
            };
            addClientProc("noParent", noParent);
        }

        //----< load Fetch Meta data of server checkedIn file processing into dispatcher dictionary >------

        private void DispatcherGetMetaData()
        {
            Action<CsMessage> getMetaData = (CsMessage rcvMsg) =>
            {
                if(test) ShowRcvMess("getMetaData");
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("error"))
                        GetMetaDataError(enumer.Current.Value);
                    if (key.Contains("filename"))
                        GetMetaDataFileName(enumer.Current.Value);
                    else if (key.Contains("descrip"))
                        GetMetaDataDescrip(enumer.Current.Value);
                    else if (key.Contains("dateTime"))
                        GetMetaDataDateTime(enumer.Current.Value);
                    else if (key.Contains("depnds"))
                        GetMetaDataDepnds(enumer.Current.Value);
                    else if (key.Contains("categ"))
                        GetMetaDataCateg(enumer.Current.Value);
                    else if (key.Contains("status"))
                        GetMetaDataStatus(enumer.Current.Value);
                    else if (key.Contains("versions"))
                        GetMetaDataVersions(enumer.Current.Value);
                }
            };
            addClientProc("getMetaData", getMetaData);
        }
        //----< Get meta data >----------------

        private void GetMetaDataError(string arg)
        {
            Action<string> mess = (string value) =>
            {
                this.statusBarText.Text = value;
            };
            Dispatcher.Invoke(mess, new Object[] { arg });
        }

        //----< Get meta data >----------------
        private void GetMetaDataFileName(string arg)
        {
            Action<string> mess = (string value) =>
            {
                if (test)
                    Console.WriteLine("FileName: " + value);
            };
            Dispatcher.Invoke(mess, new Object[] { arg });
        }

        //----< Get meta data >----------------
        private void GetMetaDataDescrip(string arg)
        {
            Action<string> mess = (string value) =>
            {
                if(test)
                    Console.WriteLine("Description: "+value);
                this.MetaDescpText.Text = value;
            };
            Dispatcher.Invoke(mess, new Object[] { arg });
        }

        //----< Get meta data >----------------

        private void GetMetaDataStatus(string arg)
        {
            Action<string> mess = (string value) =>
            {
                if (test)
                    Console.WriteLine("Status: " + value);
                
                    this.MetaDataStatus.Text = value;
            };
            Dispatcher.Invoke(mess, new Object[] { arg });
        }

        //----< Get meta data >----------------

        private void GetMetaDataDateTime(string arg)
        {
            Action<string> mess = (string value) =>
            {
                if (test)
                    Console.WriteLine("DateTime: " + value);
                
                    this.MetaDateTimeText.Text = value;
            };
            Dispatcher.Invoke(mess, new Object[] { arg });
        }

        //----< Get meta data >----------------

        private void GetMetaDataDepnds(string arg)
        {
            Action<string> mess = (string value) =>
            {
                if (test)
                    Console.WriteLine("Dependencies: " + value);
                
                {
                    string[] words = value.Split('|');
                    foreach (var word in words)
                    {
                        if (word != "")
                            VMDependentFilesList.Items.Add(word);
                    }
                }
            };
            Dispatcher.Invoke(mess, new Object[] { arg });
        }

        //----< Get meta data >----------------

        private void GetMetaDataCateg(string arg)
        {
            Action<string> mess = (string value) =>
            {
                if (test)
                    Console.WriteLine("Categories: " + value);
                
                {
                    string[] words = value.Split('|');
                    foreach (var word in words)
                    {
                        if (word != "")
                            VMCategoriesList.Items.Add(word);
                    }
                }
            };
            Dispatcher.Invoke(mess, new Object[] { arg });
        }

        //----< Get meta data >----------------

        private void GetMetaDataVersions(string arg) {
            Action<string> mess = (string value) =>
            {
                string[] words = value.Split('|');
                int idx = 0;
                for (int i = 0; i < words.Length; i++){
                    if (i == words.Length - 1){
                        MetaVersions.SelectedIndex = Int32.Parse(words[i]);
                        continue;
                    }
                    if (words[i] != ""){
                        MetaVersions.Items.Insert(idx, words[i]);
                        idx++;
                    }
                }
            };
            Dispatcher.Invoke(mess, new Object[] { arg });
        }
        //----< load all Browse dispatcher processing >---------------------------

        private void loadDispatcherBrowse()
        {
            DispatcherLoadGetDirs();
            DispatcherLoadGetFiles();
        }

        //----< load all remaining dispatcher processing >---------------------------

        private void loadDispatcher()
        {
            DispatcherLoadMakeConnection();
            DispatcherEcho();
            // load dispatcher for Browse
            loadDispatcherBrowse();
            DispatcherLoadGetFile();
            DispatcherLoadRcvFile();
            DispatcherCheckIn();
            DispatcherCheckOut();
            DispatcherGetMetaData();
            DispatcherLoadQuery();
            DispatcherNoParent();
        }
        //----< start Comm, fill window display with dirs and files >------

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            // start Comm
            endPoint_ = new CsEndPoint();
            endPoint_.machineAddress = "localhost";
            endPoint_.port = 8082;
            translater = new Translater();
            translater.listen(endPoint_);
            // start processing messages
            processMessages();

            // load dispatcher
            loadDispatcher();
            saveFilesPath = translater.setSaveFilePath("../../../SaveFiles");
            sendFilesPath = translater.setSendFilePath("../../../SendFiles");
            testMain();
        }

        //----< start test functions to demostrate OOD Project #4 requirements >------

        void testMain() {
            Thread.Sleep(1000);
            Console.WriteLine("\n-----------------------------------------------------------------------");
            Console.WriteLine("Project 4: Test cases demostration");
            test1();
            test5();
            Test2a();
            test2b();
            test2ca();
            test2cb();
            test2cc();
            test2cd();
            test2ce();
            test3a();
            test3b();
            test3c();
            test4();
            test6();
            test7();
        }
        //----< strip off name of first part of path >---------------------

        private string removeFirstDir(string path)
        {
            string modifiedPath = path;
            int pos = path.IndexOf("/");
            modifiedPath = path.Substring(pos + 1, path.Length - pos - 1);
            return modifiedPath;
        }
        //----< respond to mouse double-click on  Browse window dir name >----------------

        private void DirList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            //Check if it is empty
            if (DirList.SelectedItem == null)
                return;
            // build path for selected dir
            string selectedDir = (string)DirList.SelectedItem;
            string path;
            if (selectedDir == "..")
            {
                if (pathStack_.Count > 1)  // don't pop off "Storage"
                    pathStack_.Pop();
                else
                    return;
            }
            else
            {
                path = pathStack_.Peek() + "/" + selectedDir;
                pathStack_.Push(path);
            }
            // display path in Dir TextBlcok
            PathTextBlock.Text = removeFirstDir(pathStack_.Peek());

            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirs");
            msg.add("path", pathStack_.Peek());
            translater.postMessage(msg);

            // build message to get files and post it
            msg.remove("command");
            msg.add("command", "getFiles");
            translater.postMessage(msg);
        }
        //----< Demostrate message passing >----------------

        void test5()
        {
            Console.WriteLine("\n-----------------------------------------------------------------------");
            Console.WriteLine("Demonstrating Requirement #5 - HTTP style messages passing using asynchronous one-way messaging\n");
            Console.WriteLine("Communication is established between Client and Remote Repository Server");
            Console.WriteLine("Demostrating this test early, to make connection with the server repository and run all the test cases after");
            Console.WriteLine("Server IP: localhost");
            Console.WriteLine("Server Port: 8080");
            Console.WriteLine("Client IP: localhost");
            Console.WriteLine("Client Port: 8082");

            serverEndPoint_.machineAddress = this.IPAddrName.Text;
            serverEndPoint_.port = Int32.Parse(this.PortName.Text);
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = this.IPAddrName.Text;
            serverEndPoint.port = Int32.Parse(this.PortName.Text);
            CsMessage msg = new CsMessage();
            Console.WriteLine("Sending echo message to the Server: Message=[Connection test from Client]");
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "echo");
            msg.add("message", "Connection test from Client");
            connection_flag = true;
            connectedMsg = "Connected..";
            translater.postMessage(msg);
        }

        //----< Test case one >---------------------------------

        void test1()
        {
            Console.WriteLine("\n-----------------------------------------------------------------------");
            Console.WriteLine("Demonstrating Requirement #1\n");
            Console.WriteLine("Backend Server application is developed in std c++ libraries using Visual Studio 2017");
            Console.WriteLine("FrontEnd GUI application is developed in c# using .Net WPF framework and the translator in C++_CLI \n");
        }

        //----< test function 7 >----------------

        void test7()
        {
            Console.WriteLine("\n-----------------------------------------------------------------------");
            Console.WriteLine("Demonstrating Requirement #7 Automated unit test suite\n");
            Console.WriteLine("All the requirements of Project 4 is demostrated above from Requirement #1-#6 inclusing transmission of files");
        }

        //----< Test case to show tabs >---------------------------------

        void test4()
        {
            Console.WriteLine("\n-----------------------------------------------------------------------");
            Console.WriteLine("Demonstrating Requirement #4 Message passing communication system \n");
            Console.WriteLine("Message passing is done through socket. In connection tab of GUI, we can edit IP and PORT of the server machine to connect with it using socket");
            CsMessage msg = new CsMessage();
            Console.WriteLine("Sending echo message to the Server: Message=[Checking functionality of repository]");
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "echo");
            msg.add("message", "Checking functionality of repository");
            translater.postMessage(msg);
            Console.WriteLine("All the repository functionality is shown in test2. This just demostrate message passsing communication using SOCKET");
        }

        //----< respond to mouse click on  Connection window Connect button >----------------

        private void ConnectButton_Click(object sender, RoutedEventArgs e)
        {
            if (connection_flag)
                return;

            serverEndPoint_.machineAddress = this.IPAddrName.Text;
            serverEndPoint_.port = Int32.Parse(this.PortName.Text);

            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "connection");
            translater.postMessage(msg);
            msg.remove("command");
        }

        //----< respond to Browse tab mouse left button click >----------------

        private void Browse_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (!connection_flag)
                return;
            currentWindow = 'b';
            this.statusBarText.Text = connectedMsg;
            PathTextBlock.Text = "Storage";
            pathStack_.Clear();
            pathStack_.Push("../Storage");
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirs");
            msg.add("path", pathStack_.Peek());
            translater.postMessage(msg);
            msg.remove("command");
            msg.add("command", "getFiles");
            translater.postMessage(msg);
            msg.remove("command");
        }

        //----< respond to Connection tab Send button mouse click >----------------

        private void SendButton_Click(object sender, RoutedEventArgs e)
        {
            if (!connection_flag)
                return;

            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "echo");
            msg.add("message", this.SendMessg.Text);
            translater.postMessage(msg);
        }

        //----< test function 2a >----------------

        void Test2a()
        {
            Thread.Sleep(100);
            Console.WriteLine("\n---------------Demonstrating Requirement 2a Checkin a file --------------------------------------------------------");
            Console.WriteLine("\n File checkIn from GUI client into the remote code repository");
            Console.WriteLine("This demonstration also satisfies requirement 3 of being able to upload files into the repository");
            Console.WriteLine("This demonstration also satisfies the requirement of having a communication channel that can support passing HTTP style messages using asynchronous one-way messaging while passing messages from client to repository");
            Console.WriteLine("\nA file check-in is updated in the NoSql database with the updated value and the file is stored physically into the \"/Storage\" folder at the Server end");
            Console.Write("\n The updations to the database can also be seen on the GUI console window");
            Console.Write("\n Using ShowDB button in Connection tab or hit Empty Query from Query tab");
            Console.WriteLine("\n1.Executing an open check-in of file 'ServerPrototype.h' with description:Test CheckIn with no depedencies");
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "checkIn");
            string filepath = "../../../ServerPrototype/ServerPrototype.h";
            string name = System.IO.Path.GetFileName(filepath);
            if (System.IO.File.Exists(sendFilesPath + "/" + name))
                System.IO.File.Delete(sendFilesPath + "/" + name);
            System.IO.File.Copy(filepath, sendFilesPath + "/" + name);
            msg.add("sendingFile", name);
            msg.add("fileName", name);
            msg.add("namespace", "ServerPrototype");
            msg.add("descrp", "Test CheckIn");
            msg.add("cStatus", "OPEN");
            translater.postMessage(msg);
            Console.WriteLine("\nPlease refer query result with this in header [Test2a: ServerPrototype.h checkIn Verification query result]");
            Console.WriteLine("\nBecause of ansynchronous messaging, cannot show result after this line execution. I have inserted echoMessage in query message, this message is echoed back from server with the result which will explain about the incoming result");
            CsMessage msg1 = new CsMessage();
            msg1.add("to", CsEndPoint.toString(serverEndPoint_));
            msg1.add("from", CsEndPoint.toString(endPoint_));
            msg1.add("command", "query");
            msg1.add("queryName", "query2a.out");
            msg1.add("echoMessage", "Test2a: ServerPrototype.h checkIn Verification query result");
            translater.postMessage(msg1);
        }

        //----< Demostrate test req - File sending >----------------

        void test2b()
        {
            Thread.Sleep(100);
            Console.WriteLine("\n---------------Demonstrating Requirement 2b CheckOut a file --------------------------------------------------------");
            Console.WriteLine("\n File checkOut to GUI client location from the remote code repository");
            Console.WriteLine("\nA file check-in is updated in the NoSql database with the updated value and the file is stored physically into the \"/Storage\" folder at the Server end");
            Console.Write("\n The updations to the database can also be seen on the GUI console window");
            Console.Write("\n From CheckOut in GUI, same functionality can be performed");
            Console.WriteLine("\nExecuting file checkout first version of filename:[ServerPrototype.h] , namespace:[ServerPrototype]");
            currentWindow = 'o';
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getFile");
            msg.add("path", "../Storage/" + "ServerPrototype");
            msg.add("fileName", "ServerPrototype.h.1");
            translater.postMessage(msg);
        }

        //----< Demostrate test req - File sending >----------------

        void test3a()
        {
            Thread.Sleep(100);
            Console.WriteLine("\n---------------Demonstrating Requirement 3a Download and Upload --------------------------------------------------------");
            Console.WriteLine("\n This GUI is client program.");
            Console.WriteLine("\n UPLOADING : Demostrated Code CheckIn functionality in Requirement 2a which internally upload local client file into remote server repository system");
            Console.WriteLine("\n DOWNLOADING: Demostrated Code CheckOut functionality in Requirement 2b which internally download remote server repository file into local machine in ./CHECK_OUT_FILES folder");
        }

        //----< Demostrate test req - File sending >----------------

        void test3b()
        {
            Thread.Sleep(100);
            Console.WriteLine("\n---------------Demonstrating Requirement 3b Repository Content --------------------------------------------------------");
            Console.WriteLine("\n Demostrating Show Meta Data Functionality of Repository");
            Console.WriteLine("\n This can also be done from GUI in View MetaData tab");
            Console.WriteLine("\n Sending message to server to display MetaData of Query::Query.h.1 file");
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getMetaData");
            msg.add("filename", "Query.h");
            msg.add("namespace", "Query");
            msg.add("version", "1");
            translater.postMessage(msg);
        }

        //----< Demostrate test req - File sending >----------------

        void test3c()
        {
            Thread.Sleep(100);
            Console.WriteLine("\n---------------Demonstrating Requirement 3c Repository File with No Parent --------------------------------------------------------");
            Console.WriteLine("\n Demostrating No Parent file Functionality of Repository");
            Console.WriteLine("\n This can also be seen from GUI in Query tab with [No Parent Query] Button");
            Console.WriteLine("\n It will display all the files with no parents:");
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "noParent");
            translater.postMessage(msg);
        }

        //----< Demostrate test req - File sending >----------------

        void test6()
        {
            Thread.Sleep(100);
            Console.WriteLine("\n---------------Demonstrating Requirement 6 File Transfer --------------------------------------------------------");
            Console.WriteLine("\n Communication system which support sending and receiving blocks of bytes to support file transfer");
            Console.WriteLine("\nReceiving file filename:[DbCore.h]");
            currentWindow = 'o';
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getFile");
            msg.add("path", "../Storage/" + "DbCore");
            msg.add("fileName", "DbCore.h.1");
            translater.postMessage(msg);
        }

        //----< Demostrate test req - File sending >----------------

        void test2ca()
        {
            Thread.Sleep(100);
            Console.WriteLine("\n---------------Demonstrating Requirement 2ca Package Browsing--------------------------------------------------------");
            Console.WriteLine("\n Demostrating this requirement by triggering various queries to the server repository");
            Console.WriteLine("\nQuery 1: SHOW NAMESPACE=\"*Query*\" -> [Will show all files present inside the namespace *Query* pattern]");
            Console.WriteLine("\nPlease refer result for [Requirement 2c - Query 1 result]");
            Console.WriteLine("\nBecause of ansynchronous messaging, cannot show result after this line execution. I have inserted echoMessage in query message, this message is echoed back from server with the result which will explain about the incoming result");
            currentWindow = 'o';
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "query");
            msg.add("echoMessage", "Requirement 2c - Query 1 result");
            msg.add("queryName", "query2ca.out");
            msg.add("namespace", "Query");
            translater.postMessage(msg);
        }

        //----< test Query >----------------

        void test2cb()
        {
            Thread.Sleep(100);
            Console.WriteLine("\nQuery 2: SHOW DEPENDENCIES=\"DbCore::DbCore.cpp.1\" and  DEPENDENCIES=\"DateTime::DateTime.cpp.1\"-> [Will show all files with dependecies being both mentioned keys]");
            Console.WriteLine("\nPlease refer result for [Requirement 2c - Query 2 result]");
            Console.WriteLine("\nBecause of ansynchronous messaging, cannot show result after this line execution. I have inserted echoMessage in query message, this message is echoed back from server with the result which will explain about the incoming result");
            currentWindow = 'o';
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "query");
            msg.add("echoMessage", "Requirement 2c - Query 2 result");
            msg.add("depns", "DbCore::DbCore.cpp.1|DateTime::DateTime.cpp.1|");
            msg.add("queryName", "query2cb.out");
            translater.postMessage(msg);
        }

        //----< test Status Query >----------------

        void test2cc()
        {
            Thread.Sleep(100);
            Console.WriteLine("\nQuery 3: SHOW STATUS=\"OPEN\" -> [Will show all files with OPEN status]");
            Console.WriteLine("\nPlease refer result for [Requirement 2c - Query 3 result]");
            Console.WriteLine("\nBecause of ansynchronous messaging, cannot show result after this line execution. I have inserted echoMessage in query message, this message is echoed back from server with the result which will explain about the incoming result");
            currentWindow = 'o';
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "query");
            msg.add("echoMessage", "Requirement 2c - Query 3 result");
            msg.add("cStatus", "OPEN");
            msg.add("queryName", "query2cc.out");
            translater.postMessage(msg);
        }

        //----< test namespace query >----------------

        void test2cd()
        {
            Thread.Sleep(100);
            Console.WriteLine("\nQuery 4: SHOW NAMESPACE=\"DbCore\" AND FILENAME=\"*Edit*\" -> [Will show all files in namespace=\"*DbCore*\" and filename=\"*Edit*\"]");
            Console.WriteLine("\nPlease refer result for [Requirement 2c - Query 4 result]");
            Console.WriteLine("\nBecause of ansynchronous messaging, cannot show result after this line execution. I have inserted echoMessage in query message, this message is echoed back from server with the result which will explain about the incoming result");
            currentWindow = 'o';
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "query");
            msg.add("echoMessage", "Requirement 2c - Query 4 result");
            msg.add("namespace", "DbCore");
            msg.add("filename", "Edit");
            msg.add("queryName", "query2cd.out");
            translater.postMessage(msg);
        }

        //----< test anding Query >----------------

        void test2ce()
        {
            Thread.Sleep(100);
            Console.WriteLine("\nQuery 5: NAMESPACE=\"*Query*\" and CATEGORY=\"Query header file\" -> [Will show all files in namespace=\"*Query*\" and category=\"Query header file\"]");
            Console.WriteLine("\nPlease refer result for [Requirement 2c - Query 5 result]");
            Console.WriteLine("\nBecause of ansynchronous messaging, cannot show result after this line execution. I have inserted echoMessage in query message, this message is echoed back from server with the result which will explain about the incoming result");
            currentWindow = 'o';
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "query");
            msg.add("echoMessage", "Requirement 2c - Query 5 result");
            msg.add("namespace", "Query");
            msg.add("catgs", "Query header file|");
            msg.add("queryName", "query2ce.out");
            translater.postMessage(msg);
        }

        //----< Not impemented >----------------

        private void DisconnectButton_Click(object sender, RoutedEventArgs e)
        {

        }

        //----< respond to mouse double-click on  Browse window file name to show file content on a new window>----------------

        private void FileList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (!connection_flag)
                return;

            // build path for selected dir
            string selectedFile = pathStack_.Peek() + "/" + (string)DirList.SelectedItem;
            Console.WriteLine("++" + selectedFile);

            //string selectedDir = (string)FileList.SelectedItem;

            //file_window = new FileReadWin();
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getFile");
            msg.add("path", selectedFile);
            msg.add("fileName", (string)FileList.SelectedItem);
            translater.postMessage(msg);

            //file_window.Show();
        }

        //----< respond to left mouse click on  checkIn tab >----------------

        private void Check_In_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            currentWindow = 'c';
            this.statusBarText.Text = connectedMsg;
        }

        //----< respond to CheckIn tab mouse click on  checkIn button >----------------

        private void CheckInButton_Click(object sender, RoutedEventArgs e)
        {
            if (!connection_flag)
                return;
            if (CheckInInputFile.Text.Length == 0 || CheckInInputNameSpace.Text.Length == 0){
                this.statusBarText.Text = "File or Namespace box is empty!";
                CleanCheckInWinEntries();
                return;
            }
            this.statusBarText.Text = connectedMsg;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "checkIn");
            string filepath = CheckInInputFile.Text;
            if (!System.IO.File.Exists(filepath)) {
                this.statusBarText.Text = "File not exits to checkIn";
                return;
            }
            string name = System.IO.Path.GetFileName(filepath);
            if (System.IO.File.Exists(sendFilesPath + "/" + name))
                System.IO.File.Delete(sendFilesPath + "/" + name);
            System.IO.File.Copy(filepath, sendFilesPath + "/" + name);
            msg.add("sendingFile", name);
            msg.add("fileName", name);
            msg.add("namespace", CheckInInputNameSpace.Text);
            if(CheckInDescp.Text != "") msg.add("descrp", CheckInDescp.Text);
            msg.add("cStatus", CloseStatus.SelectedIndex.ToString());
            if (DependentFilesList.Items.Count > 0) {
                string childn = "";
                foreach (string obj in DependentFilesList.Items) {
                    childn += obj+"|";
                }
                msg.add("childs", childn);
            }
            if (CategoriesList.Items.Count > 0){
                string catgs = "";
                foreach (string obj in CategoriesList.Items){
                    catgs += obj + "|";
                }
                msg.add("catgs", catgs);
            }
            translater.postMessage(msg);
        }

        //----< respond to Add Dependency checkBox in CheckIn tab to enable Browse >----------------

        private void AddDepCheck_Click(object sender, RoutedEventArgs e)
        {
            if (!connection_flag)
                return;
            currentWindow = 'c';
            //Start browsing
            if (AddDepCheck.IsChecked == false){
                clearDirs();
                clearFiles();
                CIDirList.Background = Brushes.LightGray;
                CIFileListWin.Background = Brushes.LightGray;
                return;
            }
            CIDirList.Background = Brushes.White;
            CIFileListWin.Background = Brushes.White;

            PathTextBlock.Text = "Storage";
            pathStack_.Clear();
            pathStack_.Push("../Storage");
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirs");
            msg.add("path", pathStack_.Peek());
            translater.postMessage(msg);
            msg.remove("command");
            msg.add("command", "getFiles");
            translater.postMessage(msg);
            msg.remove("command");

        }

        //----< respond to Add button in CheckIn tab to update categories list in GUI >----------------

        private void AddCategory_Click(object sender, RoutedEventArgs e)
        {
            string category = CheckInCatText.Text;
            if (category == "")
                return;
            CheckInCatText.Text = "";
            CategoriesList.Items.Add(category);
        }

        //----< respond to mouse double click on checkIn tab Dir list >----------------

        private void CIDirList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            //Check if it is empty
            if (CIDirList.SelectedItem == null)
                return;
            // build path for selected dir
            string selectedDir = (string)CIDirList.SelectedItem;
            string path;
            if (selectedDir == ".."){
                if (pathStack_.Count > 1)  // don't pop off "Storage"
                    pathStack_.Pop();
                else
                    return;
            }
            else{
                path = pathStack_.Peek() + "/" + selectedDir;
                pathStack_.Push(path);
            }
            // display path in Dir TextBlcok
            CIPathTextBlock.Text = removeFirstDir(pathStack_.Peek());

            // build message to get dirs and post it
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirs");
            msg.add("path", pathStack_.Peek());
            translater.postMessage(msg);

            // build message to get files and post it
            msg.remove("command");
            msg.add("command", "getFiles");
            translater.postMessage(msg);
        }

        //----< respond to mouse double click on checkIn tab selected file list >----------------

        private void CISelectedFile_Checked(object sender, RoutedEventArgs e)
        {
            foreach (var item in TheList)
            {
                if (item.IsSelected == true && item.InList == false)
                {
                    item.InList = true;
                    addDependentKey((string)pathStack_.Peek(), item.TheText);
                    //DependentFilesList.Items.Add(item.TheText);
                }
                else if (item.IsSelected == false && item.InList == true) {
                    item.InList = false;
                    rmDependentKey((string)pathStack_.Peek(), item.TheText);
                    //DependentFilesList.Items.Remove(item.TheText);
                }
                DependentFilesList.Items.Refresh();
            }
        }

        //----< Add dependent key into UI list >----------------

        private void addDependentKey(string dir, string filename) {
            string key = dir.Split('/').LastOrDefault() + "::" + filename;
            DependentFilesList.Items.Add(key);
        }

        //----< Remove dependent key into UI list >----------------

        private void rmDependentKey(string dir, string filename)
        {
            string key = dir.Split('/').LastOrDefault() + "::" + filename;
            DependentFilesList.Items.Remove(key);
        }

        //----< respond to checkIn tab Browse button click to select local machine files path >----------------

        private void CheckInFile_Click(object sender, RoutedEventArgs e)
        {
            // Create OpenFileDialog 
            Microsoft.Win32.OpenFileDialog dlg = new Microsoft.Win32.OpenFileDialog();

            // Set filter for file extension and default file extension 
            dlg.DefaultExt = ".cpp";
            dlg.Filter = "CPP Files (*.cpp)|*.cpp|Header Files (*.h)|*.h|C Files (*.c)|*.c|C# Files (*.cs)|*.cs|XAML Files (*.xaml)|*.xaml";

            // Display OpenFileDialog by calling ShowDialog method 
            Nullable<bool> result = dlg.ShowDialog();
            // Get the selected file name and display in a TextBox 
            if (result == true)
            {
                // Open document 
                string filename = dlg.FileName;
                CheckInInputFile.Text = filename;
            }
        }

        //----< respond to CheckOut tab checkout button mouse left click>----------------

        private void CheckOutButton_Click(object sender, RoutedEventArgs e)
        {
            if (!connection_flag)
                return;
            if (CheckOutNameSpace.Text.Length == 0 || CheckOutFile.Text.Length == 0)
            {
                this.statusBarText.Text = "File or Namespace box is empty!";
                return;
            }
            this.statusBarText.Text = connectedMsg;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getFile");
            msg.add("path", "../Storage/"+CheckOutNameSpace.Text+"/"+ CheckOutFile.Text);
            translater.postMessage(msg);
        }

        //----< respond to mouse left click on checkOut tab selection >----------------

        private void Check_Out_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            CheckOutDirPath.Text = System.AppDomain.CurrentDomain.BaseDirectory + "..\\..\\..\\CHECK_OUT_FILES";
            this.statusBarText.Text = connectedMsg;
            currentWindow = 'o';
        }

        //----< respond to mouse left click on ViewMetaData tab on viewmeta data button >----------------

        private void View_MetaData_Click(object sender, RoutedEventArgs e)
        {
            if (!connection_flag)
                return;
            if (MetaDataNameSpace.Text.Length == 0 || MetaDataFile.Text.Length == 0)
            {
                this.statusBarText.Text = "File or Namespace box is empty!";
                return;
            }
            CleanMetaDataEntries();
            this.statusBarText.Text = connectedMsg;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getMetaData");
            msg.add("filename", MetaDataFile.Text);
            msg.add("namespace", MetaDataNameSpace.Text);
            msg.add("version", "0");
            translater.postMessage(msg);
        }

        //----< respond to mouse left click on ViewMetaData tab selection >----------------

        private void ViewMetaData_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.statusBarText.Text = connectedMsg;
        }

        //----< Clean Meta data window >----------------

        private void CleanMetaDataEntries() {
            this.MetaDescpText.Text = "";
            this.MetaDateTimeText.Text = "";
            this.MetaDataStatus.Text = "";
            this.VMCategoriesList.Items.Clear();
            this.VMDependentFilesList.Items.Clear();
            this.MetaVersions.Items.Clear();
        }

        //----< Clean Checkin window >----------------

        private void CleanCheckInWinEntries()
        {
            this.CheckInInputNameSpace.Text = "namespace";
            this.CheckInDescp.Text = "";
            this.CheckInCatText.Text = "";
            this.DependentFilesList.Items.Clear();
            this.CategoriesList.Items.Clear();
            this.CheckInInputFile.Text = "path";
            this.AddDepCheck.IsChecked = false;
            this.CIPathTextBlock.Text = "PATH";
            clearDirs();
            clearFiles();
            CIDirList.Background = Brushes.LightGray;
            CIFileListWin.Background = Brushes.LightGray;

        }

        //----< ShowDb Implementation button >----------------

        private void ShowDb_Click(object sender, RoutedEventArgs e)
        {
            if (!connection_flag)
                return;
            testQuery = false;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "query");
            msg.add("queryName", "query2ShowDb.out");
            translater.postMessage(msg);
        }

        //----< MetaVersion Implementation button >----------------

        private void MetaVersions_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (!connection_flag || true)
                return;
            CleanMetaDataEntries();
            this.statusBarText.Text = connectedMsg;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getMetaData");
            msg.add("filename", MetaDataFile.Text);
            msg.add("namespace", MetaDataNameSpace.Text);
            msg.add("version", this.MetaVersions.SelectedItem.ToString() );
            translater.postMessage(msg);
        }

        //----< Query Implementation button >----------------

        private void QueryButton_Click(object sender, RoutedEventArgs e)
        {
            if (!connection_flag)
                return;
            this.statusBarText.Text = connectedMsg;
            testQuery = false;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "query");
            if(QFileName.Text != "") msg.add("filename", QFileName.Text);
            if (QNameSpace.Text != "")  msg.add("namespace", QNameSpace.Text);
            if (QDescrip.Text != "")  msg.add("desrip", QDescrip.Text);
            if (QCatgs.Text != "")  msg.add("catgs", QCatgs.Text+"|");
            if (QDeps.Text != "")  msg.add("depns", QDeps.Text+"|");
            if (Qfrom.Text != "From")  msg.add("frmDate", Qfrom.Text);
            if (Qto.Text != "To")  msg.add("toDate", Qto.Text);
            if (!((closeStatusC.IsChecked == true) && (closeStatusO.IsChecked == true))) {
                if (closeStatusC.IsChecked == true) msg.add("cStatus", "CLOSE");
                else if (closeStatusO.IsChecked == true) msg.add("cStatus", "OPEN");
                else{
                    this.statusBarText.Text = "Check atleast one list box for Close status..";
                    return;
                }
            }
            msg.add("queryName", "queryButton.out");
            translater.postMessage(msg);
        }

        //----< NoParent Implementation button >----------------

        private void NoParentButton_Click(object sender, RoutedEventArgs e)
        {
            if (!connection_flag)
                return;
            this.statusBarText.Text = connectedMsg;
            this.NoParentResult.Items.Clear();
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "noParent");
            translater.postMessage(msg);
        }

        //----< set status for tab query >----------------

        private void Label_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            queryWin = true;
            NoParentResult.Items.Clear();
        }

        //----< set status for tab query >----------------
        private void Label_MouseLeftButtonDown_1(object sender, MouseButtonEventArgs e)
        {
            queryWin = false;
        }
    }
}
