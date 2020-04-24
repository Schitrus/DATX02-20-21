//
// Created by kirderf on 2020-04-23.
//

#ifndef DATX02_20_21_SETTINGS_H
#define DATX02_20_21_SETTINGS_H

class Settings {
    std::string name;
    int projectionIterations;
    float vorticityScale;
public:
    Settings();

    std::string getName();
    Settings withName(std::string name) const;

    int getProjectionIterations();
    Settings withProjectIterations(int projectionIterations) const;

    float getVorticityScale();
    Settings withVorticityScale(float vorticityScale) const;


private:
    Settings(const Settings* other);

};

Settings getNext();

#endif //DATX02_20_21_SETTINGS_H
