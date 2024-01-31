package org.horuscore.qt;

import android.os.Bundle;
import android.system.ErrnoException;
import android.system.Os;

import org.qtproject.qt5.android.bindings.QtActivity;

import java.io.File;

public class HorusQtActivity extends QtActivity
{
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        final File horusDir = new File(getFilesDir().getAbsolutePath() + "/.horus");
        if (!horusDir.exists()) {
            horusDir.mkdir();
        }

        super.onCreate(savedInstanceState);
    }
}
