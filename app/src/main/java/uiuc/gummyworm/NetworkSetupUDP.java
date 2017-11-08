package uiuc.gummyworm;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.ParcelFileDescriptor;

import java.net.DatagramSocket;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

public class NetworkSetupUDP {
    interface Progress {
        int ERROR = -1;
        int CONNECT_SUCCESS = 0;
        int GET_INPUT_STREAM_SUCCESS = 1;
        int PROCESS_INPUT_STREAM_IN_PROGRESS = 2;
        int PROCESS_INPUT_STREAM_SUCCESS = 3;
    }
    private String Ipv6address = "::1";
    private int portNumber;
    private boolean isInitialized;
    private Inet6Address mInet6Address;
    private DatagramSocket mDSocket;
    ParcelFileDescriptor mpfd;

    public NetworkSetupUDP(String argIpv6, int argPortNumber)
    {
        Ipv6address = argIpv6;
        portNumber = argPortNumber;
        isInitialized = false;
    }
    public Inet6Address getInet6Address(){return mInet6Address;}
    public DatagramSocket getDSocket(){return mDSocket;}
    public ParcelFileDescriptor getPFD(){return mpfd;}
    public boolean getIsInitialized(){return isInitialized;}

    /**
     * Get the device's active network status in the form of a NetworkInfo object.
     */
    public void initNetwork()
    {
        try {
            mInet6Address = (Inet6Address) InetAddress.getByName(Ipv6address);
            mDSocket = new DatagramSocket(portNumber, mInet6Address);
            mpfd = ParcelFileDescriptor.fromDatagramSocket(mDSocket);
        }
        catch (UnknownHostException e) {
            e.printStackTrace();}
        catch (SocketException e) {
            e.printStackTrace();}
    }
}

