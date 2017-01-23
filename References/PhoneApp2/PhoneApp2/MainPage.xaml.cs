using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;
using PhoneApp2.Resources;
using System.Windows;
using System.Threading;

namespace PhoneApp2
{
    public partial class MainPage : PhoneApplicationPage
    {

        // Cached Socket object that will be used by each call for the lifetime of this class
        Socket _socket = null;

        // Signaling object used to notify when an asynchronous operation is completed
        static ManualResetEvent _clientDone = new ManualResetEvent(false);

        // Define a timeout in milliseconds for each asynchronous call. If a response is not received within this 
        // timeout period, the call is aborted.
        const int TIMEOUT_MILLISECONDS = 2000;

        // The maximum size of the data buffer to use with the asynchronous socket methods
        const int MAX_BUFFER_SIZE = 2048;

        const int ECHO_PORT = 7;  // The Echo protocol uses port 7 in this sample
        const int QOTD_PORT = 17; // The Quote of the Day (QOTD) protocol uses port 17 in this sample

        // Constructor
        public MainPage()
        {
            InitializeComponent();
            
            // Sample code to localize the ApplicationBar
            //BuildLocalizedApplicationBar();
        }

        public string Connect(string hostName, int portNumber)
        {
            string result = string.Empty;

            // Create DnsEndPoint. The hostName and port are passed in to this method.
            DnsEndPoint hostEntry = new DnsEndPoint(hostName, portNumber);

            // Create a stream-based, TCP socket using the InterNetwork Address Family. 
            _socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            // Create a SocketAsyncEventArgs object to be used in the connection request
            SocketAsyncEventArgs socketEventArg = new SocketAsyncEventArgs();
            socketEventArg.RemoteEndPoint = hostEntry;

            // Inline event handler for the Completed event.
            // Note: This event handler was implemented inline in order to make this method self-contained.
            socketEventArg.Completed += new EventHandler<SocketAsyncEventArgs>(delegate(object s, SocketAsyncEventArgs e)
            {
                // Retrieve the result of this request
                result = e.SocketError.ToString();

                // Signal that the request is complete, unblocking the UI thread
                _clientDone.Set();
            });

            // Sets the state of the event to nonsignaled, causing threads to block
            _clientDone.Reset();

            // Make an asynchronous Connect request over the socket
            _socket.ConnectAsync(socketEventArg);

            // Block the UI thread for a maximum of TIMEOUT_MILLISECONDS milliseconds.
            // If no response comes back within this time then proceed
            _clientDone.WaitOne(TIMEOUT_MILLISECONDS);

            return result;
        }

        public string Send(string data)
        {
            string response = "Operation Timeout";

            // We are re-using the _socket object initialized in the Connect method
            if (_socket != null)
            {
                // Create SocketAsyncEventArgs context object
                SocketAsyncEventArgs socketEventArg = new SocketAsyncEventArgs();

                // Set properties on context object
                socketEventArg.RemoteEndPoint = _socket.RemoteEndPoint;
                socketEventArg.UserToken = null;

                // Inline event handler for the Completed event.
                // Note: This event handler was implemented inline in order 
                // to make this method self-contained.
                socketEventArg.Completed += new EventHandler<SocketAsyncEventArgs>(delegate(object s, SocketAsyncEventArgs e)
                {
                    response = e.SocketError.ToString();

                    // Unblock the UI thread
                    _clientDone.Set();
                });

                // Add the data to be sent into the buffer
                byte[] payload = Encoding.UTF8.GetBytes(data);
                socketEventArg.SetBuffer(payload, 0, payload.Length);

                // Sets the state of the event to nonsignaled, causing threads to block
                _clientDone.Reset();

                // Make an asynchronous Send request over the socket
                _socket.SendAsync(socketEventArg);

                // Block the UI thread for a maximum of TIMEOUT_MILLISECONDS milliseconds.
                // If no response comes back within this time then proceed
                _clientDone.WaitOne(TIMEOUT_MILLISECONDS);
            }
            else
            {
                response = "Socket is not initialized";
            }

            return response;
        }
        static int counter = 0;

        private bool isEoc(string _result)
        {
            if (_result.Length == 0 || _result == "close" || _result == "Operation Timeout")
            {
                return true;
            }
            return false;
        }

        private void SendCommand(string _command,string _value)
        {
            txtLog.Text = "";
            string result = Connect(txtIP.Text, 80);
           
            result = Send(_command);
            
            //result = Read();

            txtLog.Text += "'" + result + "'" + counter.ToString() + Environment.NewLine;
                 
            result = Send(_value);

            //result = Read();
            
            txtLog.Text += "'" + result + "'" + counter.ToString() + Environment.NewLine;

            switch (_command)
            { 
                case "get":

                break;
            }
                        
            switch (result)
            { 
                case "list":
                    for (int r = 0; r < 50; r++)
                    {
                        result = Read();
                        txtLog.Text += result + " " + counter.ToString() + Environment.NewLine;
                        if (isEoc(result))
                        {
                            break;
                        }
                    }
                break;
            }

            counter++;
            Close();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            // SendCommand("status","");
            SendCommand("get", "rb000");
        }
        
        static List<string> srvBuffer = new List<string>();

        public string Read()
        {
            string result;
            
            res:
            if (srvBuffer.Count() > 0)
            {
                result = srvBuffer[0];
                srvBuffer.RemoveAt(0);
                return result;
            }
            result = Receive();
            char[] chars = new char[2];
            chars[0] = (char)10;
            chars[1] = (char)13;
            string[] results = result.Split(chars[0]);
            if (results.Count() > 0)
            {
                for (int r = 0; r < results.Count(); r++)
                {
                    result = results[r].TrimEnd(chars[1]);
                    if (result != "")
                    {
                        srvBuffer.Add(results[r]);
                    }
                }

            }
            goto res;
        }

        public string Receive()
        {
            string response = "Operation Timeout";

            // We are receiving over an established socket connection
            if (_socket != null)
            {
                // Create SocketAsyncEventArgs context object
                SocketAsyncEventArgs socketEventArg = new SocketAsyncEventArgs();
                socketEventArg.RemoteEndPoint = _socket.RemoteEndPoint;

                // Setup the buffer to receive the data
                socketEventArg.SetBuffer(new Byte[MAX_BUFFER_SIZE], 0, MAX_BUFFER_SIZE);

                // Inline event handler for the Completed event.
                // Note: This even handler was implemented inline in order to make 
                // this method self-contained.
                socketEventArg.Completed += new EventHandler<SocketAsyncEventArgs>(delegate(object s, SocketAsyncEventArgs e)
                {
                    if (e.SocketError == SocketError.Success)
                    {
                        // Retrieve the data from the buffer
                        response = Encoding.UTF8.GetString(e.Buffer, e.Offset, e.BytesTransferred);
                        response = response.Trim('\0');
                    }
                    else
                    {
                        response = e.SocketError.ToString();
                    }

                    _clientDone.Set();
                });

                // Sets the state of the event to nonsignaled, causing threads to block
                _clientDone.Reset();

                // Make an asynchronous Receive request over the socket
                _socket.ReceiveAsync(socketEventArg);

                // Block the UI thread for a maximum of TIMEOUT_MILLISECONDS milliseconds.
                // If no response comes back within this time then proceed
                _clientDone.WaitOne(TIMEOUT_MILLISECONDS);
            }
            else
            {
                response = "Socket is not initialized";
            }

            return response;
        }

        /// <summary>
        /// Closes the Socket connection and releases all associated resources
        /// </summary>
        public void Close()
        {
            if (_socket != null)
            {
                _socket.Close();
            }
        }

        // waps
        // write , address , pin , vlaue
        // w / r , A , 1 , 1/0
        
        private void btn1_Click(object sender, RoutedEventArgs e)
        {
            SendCommand("set", "wb000");
        }

        private void btn2_Click(object sender, RoutedEventArgs e)
        {
            SendCommand("set", "wb010");
        }

        private void btn3_Click(object sender, RoutedEventArgs e)
        {
            SendCommand("set", "wb020");
        }

        private void btn4_Click(object sender, RoutedEventArgs e)
        {
            SendCommand("set", "wb030");
        }

        private void btn5_Click(object sender, RoutedEventArgs e)
        {
            SendCommand("set", "wb040");
        }

        private void btn6_Click(object sender, RoutedEventArgs e)
        {
            SendCommand("set", "wb050");
        }

        private void sld_r_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            //SendCommand("set", "wa00" + (char)(int)sld_r.Value);
        }

        private void sld_g_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            //SendCommand("set", "wa01" + (char)(int)sld_g.Value);
        }

        private void sld_b_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
           //SendCommand("set", "wa02" + (char)(int)sld_b.Value);
        }

        char rValue = '0';
        char gValue = '0';
        char bValue = '0';
                
        private void btn_r_on_Click(object sender, RoutedEventArgs e)
        {
            if (btn_r_on.IsChecked == true)
            {
                SendCommand("set", "wa00" + '9');
                rValue = '9';
            }
            else
            {
                SendCommand("set", "wa00" + '0');
                rValue = '0';
            }
        }
        private void btn_g_on_Click(object sender, RoutedEventArgs e)
        {
            if (btn_g_on.IsChecked == true)
            {
                SendCommand("set", "wa01" + '9');
                gValue = '9';
            }
            else
            {
                SendCommand("set", "wa01" + '0');
                gValue = '0';
            }
        }

        private void btn_b_on_Click(object sender, RoutedEventArgs e)
        {
            if (btn_b_on.IsChecked == true)
            {
                SendCommand("set", "wa02" + '9');
                bValue = '9';
            }
            else
            {
                SendCommand("set", "wa02" + '0');
                bValue = '0';
            }
        }

        private void btn_r_off_Click(object sender, RoutedEventArgs e)
        {

        }

        private void btn_g_off_Click(object sender, RoutedEventArgs e)
        {

        }

        

        private void plus_r_Click(object sender, RoutedEventArgs e)
        {
            rValue++;
            SendCommand("set", "wa00" + rValue);
        }

        private void plus_g_Click(object sender, RoutedEventArgs e)
        {
            gValue++;
            SendCommand("set", "wa01" + gValue);
        }

        private void plus_b_Click(object sender, RoutedEventArgs e)
        {
            bValue++;
            SendCommand("set", "wa02" + bValue);
        }

        private void minus_r_Click(object sender, RoutedEventArgs e)
        {
            rValue--;
            SendCommand("set", "wa00" + rValue);
        }

        private void minus_g_Click(object sender, RoutedEventArgs e)
        {
            gValue--;
            SendCommand("set", "wa01" + gValue);
        }

        private void minus_b_Click(object sender, RoutedEventArgs e)
        {
            bValue--;
            SendCommand("set", "wa02" + bValue);
        }

        private void btn_r_off_Checked(object sender, RoutedEventArgs e)
        {
            SendCommand("set", "wa00" + (byte)255);
        }


        // Sample code for building a localized ApplicationBar
        //private void BuildLocalizedApplicationBar()
        //{
        //    // Set the page's ApplicationBar to a new instance of ApplicationBar.
        //    ApplicationBar = new ApplicationBar();

        //    // Create a new button and set the text value to the localized string from AppResources.
        //    ApplicationBarIconButton appBarButton = new ApplicationBarIconButton(new Uri("/Assets/AppBar/appbar.add.rest.png", UriKind.Relative));
        //    appBarButton.Text = AppResources.AppBarButtonText;
        //    ApplicationBar.Buttons.Add(appBarButton);

        //    // Create a new menu item with the localized string from AppResources.
        //    ApplicationBarMenuItem appBarMenuItem = new ApplicationBarMenuItem(AppResources.AppBarMenuItemText);
        //    ApplicationBar.MenuItems.Add(appBarMenuItem);
        //}
    }
}