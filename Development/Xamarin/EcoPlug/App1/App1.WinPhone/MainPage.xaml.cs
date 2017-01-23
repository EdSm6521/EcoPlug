
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using System.Net.Http;
using System.Text;
using System.Windows;
using System.Threading;
using System.Threading.Tasks;

namespace App1.WinPhone
{

    class TimerExampleState
    {
        public int counter = 0;
        public Timer tmr;
    }

    public sealed partial class MainPage : Page
    {

        static HttpClient gHttpClient = new HttpClient();

        int count = 1;

        bool initialized = false;

        public MainPage()
        {

            this.InitializeComponent();

            // this.NavigationCacheMode = NavigationCacheMode.Required;

            gHttpClient.BaseAddress = new Uri("http://192.168.4.1");

            getSwitch();
            getWatts();

            textBlock.Text = "...";



        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
           
        }

        static string asyncResult = "";

        static async Task DownloadPageAsync(string _sa)
        {
            asyncResult = "";
            string page = "http://192.168.4.1" + _sa;
            HttpClient client = new HttpClient();
            using (HttpResponseMessage response = await client.PostAsync(page,null))
            using (HttpContent content = response.Content)
            {
                string result = await content.ReadAsStringAsync();
                asyncResult = result;
            }
        }

        static int ia = 0;

        private void toggleSwitch_Toggled(object sender, RoutedEventArgs e)
        {
            ia++;
            // getWatts();
            turnSwitch(toggleSwitch.IsOn);
        }

        private static bool switchOn = false;
        private async void turnSwitch(bool _on)
        {

            string value = "ON";

            switchOn = _on;
            if (!_on)
            {
                value = "OFF";
                textBlock.Text = "...";
            }
            else
            {
                textBlock.Text = "Calculando...";
            }

            await DownloadPageAsync("/switch"+value);
          
            await checkConsumption();
            
        }
        static float wattsresult = 0;
        private async void getWatts()
        {
            await DownloadPageAsync("/get_watts");
            if (asyncResult.Length != 0)
            {
                float result = (Convert.ToSingle(asyncResult));
                wattsresult = result;
            }
        }

        private void textBlock_KeyUp(object sender, KeyRoutedEventArgs e)
        {
            
        }

        private void textBlock_Tapped(object sender, TappedRoutedEventArgs e)
        {
           getWatts();
           textBlock.Text = wattsresult.ToString() + " Watts";
        }

        private void Page_Loaded(object sender, RoutedEventArgs e)
        {

        }

        private async void getSwitch()
        {
            await DownloadPageAsync("/get_switch");
            if (asyncResult == "")
            {
                //  DisplayAlert("WIFI", "Dispositivo no encontrado chequee la conexion con la red .", "OK");
                textBlock1.Text = "Comprobar Conexion WIFI , Presione para reintentar !!!";
                toggleSwitch.IsEnabled = false;
                textBlock.Text = "";
            }
            else
            {
                textBlock1.Text = "";
                toggleSwitch.IsEnabled = true;
            }
            if (asyncResult == "ON")
            {
                toggleSwitch.IsOn = true;
            }
            // textBlock1.Text = "'" + asyncResult + "'" + asyncResult.Length;
        }

        private void Page_Tapped(object sender, TappedRoutedEventArgs e)
        {
            
        }

        private void textBlock1_Tapped(object sender, TappedRoutedEventArgs e)
        {
            getSwitch();
        }

        private async Task checkConsumption()
        {
            for (int r = 0; r < 60;r++)
            {
                await Task.Delay(1000);
                if (switchOn)
                {
                    getWatts();
                    if (wattsresult != 0.0f)
                    {
                        textBlock.Text = wattsresult.ToString() + " Watts";
                        break;
                    }
                    else
                    {
                        if ((r % 2) == 0)
                        {
                            textBlock.Text = "Calculando...";
                        }
                        if ((r % 3) == 0)
                        {
                            textBlock.Text = "Calculando ..";
                        }
                        if ((r % 4) == 0)
                        {
                            textBlock.Text = "Calculando  .";
                        }
                    }
                }
                else
                {
                    break;
                }
            }
        }

    }

}
