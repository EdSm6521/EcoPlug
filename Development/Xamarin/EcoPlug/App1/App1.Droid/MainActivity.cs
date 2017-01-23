using System;

using Android.App;
using Android.Content;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using Android.OS;
using System.Net.Http;
using System.Threading.Tasks;
using System.Net.Sockets;
using System.Net;

namespace App1.Droid
{

    [Activity (Label = "EcoPlug v1.0", MainLauncher = true, Icon = "@drawable/icon")]

    public class MainActivity : Activity
	{

		int count = 1;

        TextView txtError;
        Button   btnOnOff;
        TextView txtWatts;

        private void initializeGUI()
        {
            SetContentView(Resource.Layout.Main);

            txtError = FindViewById<TextView>(Resource.Id.txtError);
            btnOnOff = FindViewById<Button>(Resource.Id.btnOnOff);
            txtWatts = FindViewById<TextView>(Resource.Id.txtWatts);

            btnOnOff.Click += new System.EventHandler(this.eventOnOff);

        }

        protected override void OnCreate (Bundle bundle)
		{

            base.OnCreate(bundle);

            initializeGUI();

            getSwitchState();

        }

        string srCommand(string _command)
        {

            string recStr = "";

            try
            {
                TcpClient tcpc = new TcpClient("192.168.4.1", 80);
                NetworkStream stream;

                //tcpc.ReceiveTimeout = 5000;
                //tcpc.SendTimeout    = 5000;

                stream = tcpc.GetStream();

                byte[] sendbuffer = cStringUtils.toByteArray(_command);

                stream.Write(sendbuffer, 0, sendbuffer.Length);
                stream = tcpc.GetStream();

                // byte[] recBuffer = new byte[256];
                // int result = stream.Read(recBuffer, 0, recBuffer.Length);
                // string recStr = cStringUtils.toString(recBuffer);

                try
                {
                    for (int r = 0; r < 256; r++)
                    {
                        byte ba = (byte)stream.ReadByte();
                        if (ba == 255) { break; }
                        recStr += (char)ba;
                    }
                }
                catch (Exception _e)
                {
                    Console.WriteLine("error : " + _e.ToString());
                }
            }
            catch (Exception _e)
            {
                txtError.Text = "Error de conexion , verifique estar conectado a la red  WIFI 'ECOPLUG0001' y que el ecoplug este enchufado";
                btnOnOff.Text = "Reintentar";
                // Console.WriteLine("error : " + _e.ToString());
            }

            Console.WriteLine("result : " + recStr);

            return recStr;
        }
      
        void getSwitchState()
        {

            string recStr = srCommand("/get_switch");

            Console.WriteLine(recStr);

            if (recStr == "ON")
            {
                btnOnOff.Text = "On";
                switchOn = true;
                checkConsumptionThread();
            }
            else
            {
                btnOnOff.Text = "Off";
                switchOn = false;
            }

        }

        static bool switchOn = false;

        private void eventOnOff(object sender, EventArgs e)
        {

            if(btnOnOff.Text == "Reintentar")
            {
                string recStr = srCommand("/get_switch");
                if (recStr != "")
                {
                    getSwitchState();
                    txtError.Text = "";
                }
            }
            else
            if (btnOnOff.Text == "Off")
            {
                srCommand("/switchON");
                btnOnOff.Text = "On";
                switchOn = true;
                checkConsumptionThread();
            }
            else
            {
                srCommand("/switchOFF");
                btnOnOff.Text = "Off";
                txtWatts.Text = "...";
                switchOn = false;
            }

        }

        private async void checkConsumptionThread()
        {
            await checkConsumption();
        }

        private async Task checkConsumption()
        {
            int r = 0;
            while (switchOn)
            {
                await Task.Delay(1000);
                {
                    r++;
                    if (r > 16) { r = 0; }
                    string wattsresult = srCommand("/get_watts");
                    Console.WriteLine("wattsresult: " + wattsresult);
                    if (!wattsresult.Contains("0."))
                    {
                        if (wattsresult != "")
                        {
                            string[] result = wattsresult.Split(' ');
                            txtWatts.Text = result[0] + " Watts";
                        }
                        break;
                    }
                    else
                    {
                        if ((r % 2) == 0)
                        {
                            txtWatts.Text = "Calculando Consumo ...";
                        }
                        if ((r % 3) == 0)
                        {
                            txtWatts.Text = "Calculando Consumo ..";
                        }
                        if ((r % 4) == 0)
                        {
                            txtWatts.Text = "Calculando Consumo  .";
                        }
                    }
                }

            }
        }


    }
}
