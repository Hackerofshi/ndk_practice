package com.tech.pratice_ffmpeg;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.os.Bundle;
import android.os.Environment;

import com.blankj.utilcode.util.LogUtils;
import com.blankj.utilcode.util.NetworkUtils;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.util.Arrays;

public class TestAVIOActivity extends AppCompatActivity {

    String path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/1/" + "20240820_03.31.48.mp4";
    private DatagramSocket datagramSocket;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test_avioactivity);
        requestPermission();

        String ipAddress = NetworkUtils.getIPAddress(true);
        LogUtils.i(ipAddress);
        new Thread(() -> {
            //Player.INSTANCE.testAVIOContext(path);
            Player.INSTANCE.startUDPClient();
        }).start();

       // receiverData();

    }

    private static final int MAX_PACKET_SIZE = 1024 * 8;
    private void receiverData() {
        new Thread(() -> {
            try {
                // 接收 UDP 数据并解码
                datagramSocket = new DatagramSocket(1234);
                byte[] buffer = new byte[MAX_PACKET_SIZE];
                DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
                while (!Thread.currentThread().isInterrupted()) {
                    datagramSocket.receive(packet);
                    int dataSize = packet.getLength();
                    byte[] data = Arrays.copyOfRange(buffer, 0, dataSize);
                    LogUtils.i("接收数据" + data.length);
                }
                LogUtils.i("receiverDataThread 停止");
            } catch (IOException e) {
                LogUtils.i("IOException 停止" + e.getMessage());
                Thread.currentThread().interrupt();
                close();
            } finally {
                LogUtils.i("finally 停止");
                close();
            }
        }).start();
    }


    public void close() {
        try {
            if (datagramSocket != null && !datagramSocket.isClosed()) {
                datagramSocket.close();
                datagramSocket.disconnect();
            }
        } catch (Exception e) {
            LogUtils.i("close" + e.getMessage());
        }
    }

    public void requestPermission() {
        ActivityCompat.requestPermissions(this,
                new String[]{Manifest.permission.CAMERA,
                        Manifest.permission.WRITE_EXTERNAL_STORAGE,
                        Manifest.permission.READ_EXTERNAL_STORAGE}, 1);
    }
}