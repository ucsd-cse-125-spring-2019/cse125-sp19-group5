#include "ConfigSettings.h"

string ConfigSettings::str_screen_width = "ScreenWidth";
string ConfigSettings::str_screen_height = "ScreenHeight";
string ConfigSettings::str_full_screen = "FullScreen";

typedef pair <string, string> String_Pair;

ConfigSettings &ConfigSettings::get() {
	static ConfigSettings instance;
	return instance;
}

ConfigSettings::ConfigSettings(string fname, string template_fname) {
	file_name = fname;
	template_file_name = template_fname;

	if(!loadSettingsFile()){ //loads settings
		copySettingsTemplate();
	}
	else{
		copyMissingSettings();
	}
}

//Reads and loads the settings
bool ConfigSettings::loadSettingsFile(){
	
	ifstream myfile;
	myfile.open(file_name.c_str());

	if(myfile.is_open()){
		string line;
		while(myfile.good()){
			getline(myfile,line);
			size_t pos = line.find_first_of(':');

			if(pos > 0 && pos < line.length()){
				string key, value;
				key = line.substr(0,pos);
				value = line.substr(pos+2);

				settings.insert(String_Pair(key, value));
			}
		}
		settings_loaded = true;
		myfile.close();
		return true;
	}

	return false;
}

//clears data read and loads from the settings file
bool ConfigSettings::reloadSettingsFile(){
	settings.clear();
	return loadSettingsFile();
}

//set the default settings from the config file
//loads the config file while copying
void ConfigSettings::copySettingsTemplate() {
	ifstream template_file;
	ofstream myfile;

	template_file.open(template_file_name.c_str());
	myfile.open(file_name.c_str());

	if(template_file.is_open()){
		string line;
		while(template_file.good()){
			getline(template_file,line);
			
			size_t pos = line.find_first_of(':');
			if(pos > 0 && pos < line.length()){
				string key, value;
				key = line.substr(0,pos);
				value = line.substr(pos+2);
			
			
				if(myfile.is_open()){
					myfile << line << endl;
				}
			
				settings.insert(String_Pair(key, value)); //loads the config file while copying
			}
		}
		settings_loaded = true;
		template_file.close();
		myfile.close();
	}
}

void ConfigSettings::copyMissingSettings() {
	
	ifstream template_file;
	fstream myfile;

	template_file.open(template_file_name.c_str());
	myfile.open(file_name.c_str(), fstream::in | fstream::out | fstream::app);

	if(template_file.is_open()){
		string line;
		while(template_file.good()){
			getline(template_file,line);
			
			size_t pos = line.find_first_of(':');
			if(pos > 0 && pos < line.length()){
				string key, value;
				key = line.substr(0,pos);
				value = line.substr(pos+2);
			
				auto i = settings.find(key);
	
				if(i == settings.end()){
					if(myfile.is_open()){
						myfile << line << endl;
					}
					settings.insert(String_Pair(key, value));
				}
			}
		}
		template_file.close();
		myfile.close();
	}

}

void ConfigSettings::saveSettingsFile(){

	ofstream myfile;

	myfile.open(file_name.c_str());

	if(myfile.is_open()){
		string line;
		auto endIter = settings.end();
		
		for(auto iter = settings.begin(); iter != endIter; iter++){
			myfile << iter->first << ": " << iter->second << endl;
		}

		myfile.close();
	}

}

bool ConfigSettings::getValue(string key, string & ret){
	auto i = settings.find(key);
	
	if(i != settings.end()){
		ret = i->second;
		return true;
	}
	return false;
}

bool ConfigSettings::getValue(string key, bool & ret){
	auto i = settings.find(key);
	
	if(i != settings.end()){
		if(i->second == "true"){
			ret = true;
		}else{
			ret = false;
		}
		return true;
	}
	return false;
}

bool ConfigSettings::getValue(string key, int & ret){
	auto i = settings.find(key);
	
	if(i != settings.end()){
		ret = atoi(i->second.c_str()); //error results in 0
		return true;
	}
	return false;
}

bool ConfigSettings::getValue(string key, float & ret){
	auto i = settings.find(key);
	
	if(i != settings.end()){
		ret = (float) atof(i->second.c_str()); //error results in 0
		return true;
	}
	return false;
}

bool ConfigSettings::getValue(string key, double & ret){
	auto i = settings.find(key);
	
	if(i != settings.end()){
		ret = atof(i->second.c_str()); //error results in 0
		return true;
	}
	return false;
}

void ConfigSettings::updateValue(string key, string value){
	settings.insert(String_Pair(key, value));
}


void ConfigSettings::updateValue(string key, bool value){
	if(value){
		settings.insert(String_Pair(key, "true"));
	}else{
		settings.insert(String_Pair(key, "false"));
	}
}

void ConfigSettings::updateValue(string key, int value){
	stringstream s;
	s << value;
	
	settings.insert(String_Pair(key, s.str()));
}


void ConfigSettings::updateValue(string key, float value){
	stringstream s;
	s << value;
	
	settings.insert(String_Pair(key, s.str()));
}

void ConfigSettings::updateValue(string key, double value){
	stringstream s;
	s << value;
	
	settings.insert(String_Pair(key, s.str()));
}

//returns true if loaded the saved settings, otherwise false
bool ConfigSettings::checkIfLoaded() {
	return settings_loaded;
}