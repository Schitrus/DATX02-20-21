package com.pbf;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.SeekBar;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProviders;

import com.example.datx02_20_21.R;

public class SettingsFragment extends Fragment {

    private static final int VORTICITY_MIN = 1;
    private static final int VORTICITY_MAX = 10;

    private SeekBar vorticityBar;
    private TextView vorticityText;

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.settings_fragment, container, false);
        initUIElements(view);
        return view;
    }

    private void initUIElements(View v){
        vorticityBar = v.findViewById(R.id.vorticityBar);
        vorticityText = v.findViewById(R.id.vorticityText);

        vorticityBar.setMax(10);
        vorticityBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                if(i < VORTICITY_MIN){
                    i = VORTICITY_MIN;
                }
                vorticityText.setText(String.valueOf(i));
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }

}
