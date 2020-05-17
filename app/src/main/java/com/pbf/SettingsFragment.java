package com.pbf;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import com.example.datx02_20_21.R;

import java.math.BigDecimal;
import java.math.RoundingMode;

public class SettingsFragment extends Fragment {

    private static final int VORTICITY_MIN = 0;
    private static final int VORTICITY_MAX = 1000;
    private static final float VORTICITY_STEP = 0.05f;
    private static final int ITERATIONS_MIN = 1;
    private static final int ITERATIONS_MAX = 100;
    private static final float ITERATIONS_STEP = 1.0f;
    private static final int BUOYANCY_MIN = 0;
    private static final int BUOYANCY_MAX = 100;
    private static final float BUOYANCY_STEP = 0.1f;
    private static final int VISCOSITY_MIN = 1;
    private static final int VISCOSITY_MAX = 1000;
    private static final float VISCOSITY_STEP = 0.01f;
    private static final int WIND_MIN = 0;
    private static final int WIND_MAX = 1000;
    private static final float WIND_STEP = 0.25f;

    private enum ResolutionItems{
            LOW, MEDIUM, HIGH;

        public static String[] stringValues(){
                ResolutionItems[] resolutionItems = ResolutionItems.values();
                String[] values = new String[resolutionItems.length];
                for(int i = 0; i < resolutionItems.length; i++){
                    values[i] = resolutionItems[i].name();
                }
                return values;
        }
    };

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

    private Spinner resolutionSpinner;

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
        initResolutionSpinner(v);
    }

    private void initVorticityBar(View v){
        vorticityBar = v.findViewById(R.id.vorticityBar);
        vorticityValueText = v.findViewById(R.id.vorticityValueText);

        vorticityBar.setOnSeekBarChangeListener(
                new SliderBarListener(vorticityBar, vorticityValueText, VORTICITY_MIN, VORTICITY_MAX, VORTICITY_STEP)
        );
    }

    private void initIterationsBar(View v){
        iterationsBar = v.findViewById(R.id.iterationsBar);
        iterationsValueText = v.findViewById(R.id.iterationsValueText);

       iterationsBar.setOnSeekBarChangeListener(
               new SliderBarListener(iterationsBar, iterationsValueText, ITERATIONS_MIN, ITERATIONS_MAX, ITERATIONS_STEP)
       );
    }

    private void initBuoyancyBar(View v){
        buoyancyBar = v.findViewById(R.id.buoyancyBar);
        buoyancyValueText = v.findViewById(R.id.buoyancyValueText);

        buoyancyBar.setOnSeekBarChangeListener(
                new SliderBarListener(buoyancyBar, buoyancyValueText, BUOYANCY_MIN, BUOYANCY_MAX, BUOYANCY_STEP)
        );
    }

    private void initViscosityBar(View v){
        viscosityBar = v.findViewById(R.id.viscosityBar);
        viscosityValueText = v.findViewById(R.id.viscosityValueText);

        viscosityBar.setOnSeekBarChangeListener(
                new SliderBarListener(viscosityBar, viscosityValueText, VISCOSITY_MIN, VISCOSITY_MAX, VISCOSITY_STEP)
        );
    }

    private void initWindBar(View v){
        windBar = v.findViewById(R.id.windBar);
        windValueText = v.findViewById(R.id.windValueText);

        windBar.setOnSeekBarChangeListener(
                new SliderBarListener(windBar, windValueText, WIND_MIN, WIND_MAX, WIND_STEP)
        );
    }

    private void initResolutionSpinner(View v) {
        resolutionSpinner = v.findViewById(R.id.resolutionSpinner);


        ArrayAdapter<String> adapter = new ArrayAdapter<String>(
                getActivity(),
                android.R.layout.simple_spinner_item,
                ResolutionItems.stringValues()
        );

        // Set to this resource layout to remove radio button from menu
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        resolutionSpinner.setAdapter(adapter);
        resolutionSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int pos, long id) {
                ResolutionItems res = ResolutionItems.values()[pos];
                switch(res){
                    case LOW:
                        // Set low resolution
                        break;
                    case HIGH:
                        // Set high resolution
                        break;
                    default:
                        // Medium or anything else
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });
    }

    private static class SliderBarListener implements SeekBar.OnSeekBarChangeListener{

        private SeekBar seekBar;
        private TextView seekBarText;
        private final int minValue;
        private final int maxValue;
        private final float step;
        private int value;

        public SliderBarListener(SeekBar seekBar, TextView seekBarText, int minValue, int maxValue, float step){
            this.minValue = minValue;
            this.maxValue = maxValue;
            this.step = step;
            this.seekBar = seekBar;
            this.seekBarText = seekBarText;
            seekBar.setMax(maxValue);

            seekBarText.setText(String.valueOf(minValue));
        }

        @Override
        public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
            if(i < minValue){
                i = minValue;
            }
            value = i;
            seekBarText.setText(String.valueOf(round(value*step, 2)));
        }

        private static double round(double value, int places) {
            if (places < 0) throw new IllegalArgumentException();

            BigDecimal bd = new BigDecimal(Double.toString(value));
            bd = bd.setScale(places, RoundingMode.HALF_UP);
            return bd.doubleValue();
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {
        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
            switch (seekBar.getId()){
                case R.id.windBar:
                    updateWind((value*step));
                case R.id.vorticityBar:
                    updateVorticity((value*step));
            }
        }

        public native void updateWind(float strength);
        public native void updateVorticity(float vorticityScale);
    }
}
