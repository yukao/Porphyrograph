///////////////////////////////////////////////////////////////////////////////////
// MANAGES EXCLUSIVE BUTTONS IN INTERFACE
template <typename T>
void ExclusiveButtonsAndLabelsOnOff(vector<string> ButtonPaths, vector<string> ButtonLabelPaths, vector<T> ButtonValues, bool withDefault, T value) {
	if (ButtonPaths.size() != ButtonValues.size() || ButtonLabelPaths.size() != ButtonValues.size()) {
		sprintf(ErrorStr, "Incorrect button values and sizes (%lu/%lu/%lu) differ!", ButtonPaths.size(), ButtonLabelPaths.size(), ButtonValues.size()); ReportError(ErrorStr);  throw 768;
	}
	// all greyed except the active one
	for (int ind = 0; ind < int(ButtonPaths.size()); ind++) {
		if (fabs(value - ButtonValues[ind]) < 0.00001) {
			int selectedButtonIndex = ind;
			for (int indAux = 0; indAux < int(ButtonPaths.size()); indAux++) {
				if (indAux == selectedButtonIndex) {
					sprintf(AuxString, "%s/textColor FF8888FF", ButtonLabelPaths[indAux].c_str()); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
					sprintf(AuxString, "%s/color FF0000FF", ButtonPaths[indAux].c_str()); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
				}
				else {
					sprintf(AuxString, "%s/textColor AAAAAAFF", ButtonLabelPaths[indAux].c_str()); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
					sprintf(AuxString, "%s/color 888888FF", ButtonPaths[indAux].c_str()); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
				}
			}
			return;
		}
	}
	// no active one and default allowed => all greyed
	if (withDefault) {
		for (int ind = 0; ind < int(ButtonPaths.size()); ind++) {
			sprintf(AuxString, "%s/textColor AAAAAAFF", ButtonLabelPaths[ind].c_str()); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
			sprintf(AuxString, "%s/color 888888FF", ButtonPaths[ind].c_str()); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}
