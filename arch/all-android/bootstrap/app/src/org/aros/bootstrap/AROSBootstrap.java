package org.aros.bootstrap;

import java.io.FileDescriptor;
import java.nio.ByteBuffer;

import android.app.Application;
import android.content.Intent;
import android.graphics.Bitmap;
import android.os.Environment;
import android.os.Handler;
import android.util.Log;

final class BitmapData
{
	int	Left;
	int	Top;
	int	Width;
	int	Height;
	int BytesPerRow;
	int Address;
}

public class AROSBootstrap extends Application
{
	public int DisplayWidth;
	public int DisplayHeight;
	public BitmapData Bitmap;
	public AROSActivity ui;
	private DisplayServer Server;
	public Boolean started = false;
	private Handler handler;

	// Commands sent to us by AROS display driver
	static final int cmd_Nak    = -1;
	static final int cmd_Query  = 0x80000001;
	static final int cmd_Show   = 0x80000002;
	static final int cmd_Update = 0x00000003;
	static final int cmd_Scroll = 0x00000004;
	static final int cmd_Mouse  = 0x00000005;
	static final int cmd_Touch  = 0x00000006;
	static final int cmd_Key    = 0x00000007;
	static final int cmd_Flush  = 0x00000008;

	@Override
	public void onCreate()
	{
        Log.d("AROS", "Started");
        started = false;
	}

	@Override
	public void onLowMemory()
	{
		Log.d("AROS", "Memory panic");

		Server.ReplyCommand(cmd_Flush);
	}

	public void ColdBoot()
	{
	    // Get external storage path 
	    String arosdir = Environment.getExternalStorageDirectory().getAbsolutePath() + "/AROS";

	    Log.d("AROS", "Loading AROS, root path: " + arosdir);

	    int rc = Load(arosdir);

	    if (rc == 0)
	    	WarmBoot();
	}

	private void WarmBoot()
	{
	    FileDescriptor readfd  = new FileDescriptor();
	    FileDescriptor writefd = new FileDescriptor();
	    
	    Log.d("AROS", "Starting AROS...");

		int rc = Kick(readfd, writefd);

		if (rc == 0)
		{
			started = true;

			// Initialize and run display server thread
	        AROSBootstrap.this.Server = new DisplayServer(AROSBootstrap.this, readfd, writefd);
	        AROSBootstrap.this.Server.start();
		}
	}

	private void Reset()
	{
		// Drop the bitmap and signal this to UI
		Bitmap = null;
		if (ui != null)
			ui.Show(0, null);
	}

	// Make sure the activity is visible
	private void Foreground()
	{
    	Intent myIntent = new Intent(this, AROSActivity.class);
    	startActivity(myIntent);
	}
	
    public void DisplayError(String text)
    {
    	Foreground();
    	ui.DisplayError(text);
    }

	private void DoCommand(int cmd, int[] params)
	{
		switch (cmd)
		{
		case cmd_Query:
			Log.d("AROS", "cmd_Query(" + params[0] + ")");

			Server.ReplyCommand(cmd, DisplayWidth, DisplayHeight);
			break;

		case cmd_Show:
			Log.d("AROS", "cmd_Show(" + params[0] + ", " + params[6] + ")");

			if (params[6] == 0)
			{
				// Drop the displayed bitmap (if any)
				Bitmap = null;
			}
			else
			{
				// Create Java reflection of AROS bitmap
				Bitmap = new BitmapData();

				Bitmap.Left		   = params[1];
				Bitmap.Top		   = params[2];
				Bitmap.Width	   = params[3];
				Bitmap.Height	   = params[4];
				Bitmap.BytesPerRow = params[5];
				Bitmap.Address     = params[6];
			}

			if (ui != null)
			{
				// Signal the view to update
				ui.Show(params[0], Bitmap);
			}

			Server.ReplyCommand(cmd);
			break;

		case cmd_Update:
			if (ui != null)
			{
				BitmapView view = ui.GetBitmap(params[0]);

				view.Update(Bitmap, params[1], params[2], params[3], params[4]);
			}
			// This command doesn't need a reply
			break;

		default:
			Log.d("AROSDisplay", "Unknown command " + cmd);

			Server.ReplyCommand(cmd_Nak, cmd);
			break;
		}
	}

	public void ReportMouse(int x, int y, int action)
	{
		Server.ReplyCommand(cmd_Mouse, x, y, action);
	}

	public void ReportTouch(int x, int y, int action)
	{
		Server.ReplyCommand(cmd_Touch, x, y, action);
	}

	public void ReportKey(int code, int flags)
	{
		Server.ReplyCommand(cmd_Key, code, flags);
	}
	
    // This orders processing of a command from server
    class ServerCommand implements Runnable
    {
    	private int Command;
    	private int[] Parameters;

    	public ServerCommand(int cmd, int... params)
    	{
    		Command = cmd;
    		Parameters = params;
    	}

    	public void run()
    	{
    		// It's tedious to type AROSBootstrap.this. every time,
    		// so i simply forward the event to outer class.
			AROSBootstrap.this.DoCommand(Command, Parameters);
    	}
    }

    class WarmReboot implements Runnable
    {
    	public void run()
    	{
    		AROSBootstrap.this.Reset();
    		AROSBootstrap.this.WarmBoot();
    	}
    }

    class ColdReboot implements Runnable
    {
    	public void run()
    	{
    		AROSBootstrap.this.Reset();
    		AROSBootstrap.this.ColdBoot();
    	}
    }

    static
    {
        System.loadLibrary("AROSBootstrap");
    }
	
	// libAROSBootstrap native methods
    private native int Load(String dir);
    private native int Kick(FileDescriptor rfd, FileDescriptor wfd);
    private native ByteBuffer MapMemory(int addr, int size);
    public native int GetBitmap(Bitmap obj, int addr, int x, int y, int width, int height, int bytesPerLine);
}
