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

import com.example.datx02_20_21.R;

public class SettingsFragment extends Fragment {

    private static final int VORTICITY_MIN = 0;
    private static final int VORTICITY_MAX = 10;
    private static final int ITERATIONS_MIN = 1;
    private static final int ITERATIONS_MAX = 100;
    private static final int BUOYANCY_MIN = 0;
    private static final int BUOYANCY_MAX = 10;
    private static final int VISCOSITY_MIN = 1;
    private static final int VISCOSITY_MAX = 10;
    private static final int WIND_MIN = 0;
    private static final int WIND_MAX = 10;

    private SeekBar vorticityBar;
    private TextView vorticityValueText;

    private SeekBar iterationsBar;
    private TextView iterationsValueText;

    private SeekBar buoyancyBar;
    private TextView buoyancyValueText;

    private SeekBar viscosityBar;
    private TextView viscosityValueText;

    private SeekBar windBar;
    private TextView windValueText;

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.settings_fragment, container, false);
        initUIElements(view);
        return view;
    }

    private void initUIElements(View v){
        initVorticityBar(v);
        initIterationsBar(v);
        initBuoyancyBar(v);
        initViscosityBar(v);
        initWindBar(v);
    }

    private void initVorticityBar(View v){
        vorticityBar = v.findViewById(R.id.vorticityBar);
        vorticityValueText = v.findViewById(R.id.vorticityValueText);

        vorticityBar.setOnSeekBarChangeListener(
                new SliderBarListener(vorticityBar, vorticityValueText, VORTICITY_MIN, VORTICITY_MAX)
        );
    }

    private void initIterationsBar(View v){
        iterationsBar = v.findViewById(R.id.iterationsBar);
        iterationsValueText = v.findViewById(R.id.iterationsValueText);

       iterationsBar.setOnSeekBarChangeListener(
               new SliderBarListener(iterationsBar, iterationsValueText, ITERATIONS_MIN, ITERATIONS_MAX)
       );
    }

    private void initBuoyancyBar(View v){
        buoyancyBar = v.findViewById(R.id.buoyancyBar);
        buoyancyValueText = v.findViewById(R.id.buoyancyValueText);

        buoyancyBar.setOnSeekBarChangeListener(
                new SliderBarListener(buoyancyBar, buoyancyValueText, BUOYANCY_MIN, BUOYANCY_MAX)
        );
    }

    private void initViscosityBar(View v){
        viscosityBar = v.findViewById(R.id.viscosityBar);
        viscosityValueText = v.findViewById(R.id.viscosityValueText);

        viscosityBar.setOnSeekBarChangeListener(
                new SliderBarListener(viscosityBar, viscosityValueText, VISCOSITY_MIN, VISCOSITY_MAX)
        );
    }

    private void initWindBar(View v){
        windBar = v.findViewById(R.id.windBar);
        windValueText = v.findViewById(R.id.windValueText);

        windBar.setOnSeekBarChangeListener(
                new SliderBarListener(windBar, windValueText, WIND_MIN, WIND_MAX)
        );
    }

    private static class SliderBarListener implements SeekBar.OnSeekBarChangeListener{

        private SeekBar seekBar;
        private TextView seekBarText;
        private final int minValue;
        private final int maxValue;

        public SliderBarListener(SeekBar seekBar, TextView seekBarText, int minValue, int maxValue){
            this.minValue = minValue;
            this.maxValue = maxValue;
            this.seekBar = seekBar;
            this.seekBarText = seekBarText;
            seekBar.setMax(maxValue);
        }

        @Override
        public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
            if(i < minValue){
                i = minValue;
            }
            seekBarText.setText(String.valueOf(i));
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {

        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {

        }
    }

}
