#include "../Headers/config_info.h"

ConfigInfo::ConfigInfo(string fileName)
{
	try
	{
		cfg.readFile(fileName.c_str());
		readValues();
	}
	catch (const FileIOException &fioex)
	{
		throw "I/O error while reading config file";
	}
	catch (const ParseException &pex)
	{
		string message = "Parse error at ";
		message += pex.getFile();
		message += ":";
		message += pex.getLine();
		message += " - ";
		message += pex.getError();
		message += '\n';

		throw message.c_str();
	}
}

void ConfigInfo::readValues()
{
	const Setting& root = cfg.getRoot();

	try
	{
		const Setting& values = root[OUTER_VAR_NAME.c_str()][INNER_VAR_NAME.c_str()];

		for (int i = 0; i < values.getLength(); ++i)
		{
			const Setting& value = values[i];

			// get values
			if (!(value.lookupValue("nx", this->nx)
				&& value.lookupValue("ny", this->ny)))
				continue;
		}
	}
	catch (const SettingNotFoundException &nfex)
	{
		throw "Can't get values in the config file.";
	}
}

string ConfigInfo::getNx() const
{
	return nx;
}

string ConfigInfo::getNy() const
{
	return ny;
}


