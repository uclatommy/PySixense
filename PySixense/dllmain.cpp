#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/class.hpp>
#include <boost/python/object.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/list.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/enum.hpp>
#include <sixense.h>

//DLL import definition
typedef int(*dll_sixenseInit)(void);
typedef int(*dll_sixenseExit)(void);
typedef int(*dll_sixenseGetAllNewestData)(sixenseAllControllerData *);
typedef int(*dll_sixenseGetFilterEnabled)(int *on_or_off);
typedef int(*dll_sixenseGetFilterParams)(float *near_range, float *near_val, float *far_range, float *far_val);
typedef int(*dll_sixenseGetHighPriorityBindingEnabled)(int *on_or_off);
typedef int(*dll_sixenseGetBaseColor)(unsigned char *red, unsigned char *green, unsigned char *blue);
typedef int(*dll_sixenseGetData)(int which, int index_back, sixenseControllerData *);
typedef int(*dll_sixenseGetMaxBases)();
typedef int(*dll_sixenseSetActiveBase)(int i);
typedef int(*dll_sixenseIsBaseConnected)(int i);
typedef int(*dll_sixenseGetMaxControllers)(void);
typedef int(*dll_sixenseGetNumActiveControllers)();
typedef int(*dll_sixenseIsControllerEnabled)(int which);
typedef int(*dll_sixenseGetHistorySize)();
typedef int(*dll_sixenseSetFilterEnabled)(int on_or_off);
typedef int(*dll_sixenseSetFilterParams)(float near_range, float near_val, float far_range, float far_val);
typedef int(*dll_sixenseTriggerVibration)(int controller_id, int duration_100ms, int pattern_id);
typedef int(*dll_sixenseAutoEnableHemisphereTracking)(int which_controller);
typedef int(*dll_sixenseSetHighPriorityBindingEnabled)(int on_or_off);
typedef int(*dll_sixenseSetBaseColor)(unsigned char red, unsigned char green, unsigned char blue);

dll_sixenseInit HydraInit;
dll_sixenseExit HydraExit;
dll_sixenseGetAllNewestData HydraGetAllNewestData;
dll_sixenseGetFilterEnabled HydraGetFilterEnabled;
dll_sixenseGetFilterParams HydraGetFilterParams;
dll_sixenseGetHighPriorityBindingEnabled HydraGetHighPriorityBindingEnabled;
dll_sixenseGetBaseColor HydraGetBaseColor;
dll_sixenseGetData HydraGetData;
dll_sixenseGetMaxBases HydraGetMaxBases;
dll_sixenseSetActiveBase HydraSetActiveBase;
dll_sixenseIsBaseConnected HydraIsBaseConnected;
dll_sixenseGetMaxControllers HydraGetMaxControllers;
dll_sixenseGetNumActiveControllers HydraGetNumActiveControllers;
dll_sixenseIsControllerEnabled HydraIsControllerEnabled;
dll_sixenseGetHistorySize HydraGetHistorySize;
dll_sixenseSetFilterEnabled HydraSetFilterEnabled;
dll_sixenseSetFilterParams HydraSetFilterParams;
dll_sixenseTriggerVibration HydraTriggerVibration;
dll_sixenseAutoEnableHemisphereTracking HydraAutoEnableHemisphereTracking;
dll_sixenseSetHighPriorityBindingEnabled HydraSetHighPriorityBindingEnabled;
dll_sixenseSetBaseColor HydraSetBaseColor;

// returns: success, on_or_off (bool)
static boost::python::tuple GetFilterEnabled()
{
	int on_or_off = 0;
	int success = HydraGetFilterEnabled(&on_or_off);
	return boost::python::make_tuple(success, on_or_off == 1);
}

// returns: success, near_range, near_val, far_range, far_val (floats)
static boost::python::tuple GetFilterParams()
{
	float near_range, near_val, far_range, far_val;
	int success = HydraGetFilterParams(&near_range, &near_val, &far_range, &far_val);
	return boost::python::make_tuple(success, near_range, near_val, far_range, far_val);
}

// returns success, on_or_off (bool)
static boost::python::tuple GetHighPriorityBindingEnabled()
{
	int on_or_off;
	int success = HydraGetHighPriorityBindingEnabled(&on_or_off);
	return boost::python::make_tuple(success, on_or_off == 1);
}

// returns success, red, green, blue (uchar)
static boost::python::tuple GetBaseColor()
{
	unsigned char red, green, blue;
	int success = HydraGetBaseColor(&red, &green, &blue);
	return boost::python::make_tuple(success, red, green, blue);
}

struct ControllerData
{
	boost::python::list pos;
	boost::python::list rot_mat;
	float joystick_x;
	float joystick_y;
	float trigger;
	unsigned int buttons;
	unsigned char sequence_number;
	boost::python::list rot_quat;
	unsigned short firmware_revision;
	unsigned short hardware_revision;
	unsigned short packet_type;
	unsigned short magnetic_frequency;
	bool enabled;
	int controller_index;
	bool is_docked;
	unsigned char which_hand;

	ControllerData(const sixenseControllerData& data)
	{
		for(int i = 0; i < 3; ++i)
		{
			pos.append(data.pos[i]);
		}
		for(int y = 0; y < 3; ++y)
		{
			boost::python::list col;
			for(int x = 0; x < 3; ++x)
			{
				col.append(data.rot_mat[y][x]);
			}
			rot_mat.append(col);
		}
		joystick_x = data.joystick_x;
		joystick_y = data.joystick_y;
		trigger = data.trigger;
		buttons = data.buttons;
		sequence_number = data.sequence_number;
		for(int i = 0; i < 4; ++i)
		{
			rot_quat.append(data.rot_quat[i]);
		}
		firmware_revision = data.firmware_revision;
		hardware_revision = data.hardware_revision;
		packet_type = data.packet_type;
		magnetic_frequency = data.magnetic_frequency;
		enabled = data.enabled == 1;
		controller_index = data.controller_index;
		is_docked = data.is_docked == 1;
		which_hand = data.which_hand;
	}
};

// returns success, ControllerData
static boost::python::tuple GetData(int which, int index_back)
{
	sixenseControllerData sixenseData;
	int success = HydraGetData(which, index_back, &sixenseData);
	ControllerData data(sixenseData);
	return boost::python::make_tuple(success, data);
}

// returns success, ControllerData
static boost::python::tuple GetNewestData(int which)
{
	return GetData(which, 0);
}

// returns success, list of ControllerData
static boost::python::tuple GetAllData(int index_back)
{
	boost::python::list datas;
	int success = SIXENSE_SUCCESS;
	for(int i = 0; (i < 4) && (success == SIXENSE_SUCCESS); ++i)
	{
		boost::python::tuple retVal = GetData(i, index_back);
		int success = boost::python::extract<int>(retVal[0]);
		ControllerData& data = boost::python::extract<ControllerData&>(retVal[1]);
		datas.append(data);
	}
	return boost::python::make_tuple(success, datas);
}

// returns success, list of ControllerData
static boost::python::tuple GetAllNewestData()
{
	return GetAllData(0);
}

BOOST_PYTHON_MODULE(PySixense)
{
    using namespace boost::python;

	enum Constants
	{
		Failure = SIXENSE_FAILURE,
		Success = SIXENSE_SUCCESS,
		Button1 = SIXENSE_BUTTON_1,
		Button2 = SIXENSE_BUTTON_2,
		Button3 = SIXENSE_BUTTON_3,
		Button4 = SIXENSE_BUTTON_4,
		ButtonStart = SIXENSE_BUTTON_START,
		ButtonBumper = SIXENSE_BUTTON_BUMPER,
		ButtonJoystick = SIXENSE_BUTTON_JOYSTICK,
	};

	enum_<Constants>("Constants")
		.value("Failure", Failure)
		.value("Success", Success)
		.value("Button1", Button1)
		.value("Button2", Button2)
		.value("Button3", Button3)
		.value("Button4", Button4)
		.value("ButtonStart", ButtonStart)
		.value("ButtonBumper", ButtonBumper)
		.value("ButtonJoystick", ButtonJoystick)
		;

	//class_<sixenseControllerData>("ControllerData", no_init) //no constructors please
	//	.def_readonly("joystick_x", &sixenseControllerData::joystick_x)
	//	;
	class_<ControllerData>("ControllerData", boost::python::no_init)
		.def_readonly("pos", &ControllerData::pos)
		.def_readonly("rot_mat", &ControllerData::rot_mat)
		.def_readonly("joystick_x", &ControllerData::joystick_x)
		.def_readonly("joystick_y", &ControllerData::joystick_y)
		.def_readonly("trigger", &ControllerData::trigger)
		.def_readonly("buttons", &ControllerData::buttons)
		.def_readonly("sequence_number", &ControllerData::sequence_number)
		.def_readonly("rot_quat", &ControllerData::rot_quat)
		.def_readonly("firmware_revision", &ControllerData::firmware_revision)
		.def_readonly("hardware_revision", &ControllerData::hardware_revision)
		.def_readonly("packet_type", &ControllerData::packet_type)
		.def_readonly("magnetic_frequency", &ControllerData::magnetic_frequency)
		.def_readonly("enabled", &ControllerData::enabled)
		.def_readonly("controller_index", &ControllerData::controller_index)
		.def_readonly("is_docked", &ControllerData::is_docked)
		.def_readonly("which_hand", &ControllerData::which_hand)
		;

	def("Init", HydraInit);
	def("Exit", HydraExit);
	def("GetMaxBases", HydraGetMaxBases);
	def("SetActiveBase", HydraSetActiveBase);
	def("IsBaseConnected", HydraIsBaseConnected);
	def("GetMaxControllers", HydraGetMaxControllers);
	def("GetNumActiveControllers", HydraGetNumActiveControllers);
	def("IsControllerEnabled", HydraIsControllerEnabled);
	def("GetHistorySize", HydraGetHistorySize);
	def("SetFilterEnabled", HydraSetFilterEnabled);
	//def("GetFilterEnabled", sixenseGetFilterEnabled);
	def("GetFilterEnabled", GetFilterEnabled);
	def("SetFilterParams", HydraSetFilterParams);
	//def("GetFilterParams", sixenseGetFilterParams);
	def("GetFilterParams", GetFilterParams);
	def("TriggerVibration", HydraTriggerVibration);
	def("AutoEnableHemisphereTracking", HydraAutoEnableHemisphereTracking);
	def("SetHighPriorityBindingEnabled", HydraSetHighPriorityBindingEnabled);
	//def("GetHighPriorityBindingEnabled", sixenseGetHighPriorityBindingEnabled);
	def("GetHighPriorityBindingEnabled", GetHighPriorityBindingEnabled);
	def("SetBaseColor", HydraSetBaseColor);
	//def("GetBaseColor", sixenseGetBaseColor);
	def("GetBaseColor", GetBaseColor);

	//def("GetAllNewestData", sixenseGetAllNewestData);
	//def("GetAllData", sixenseGetAllData);
	//def("GetNewestData", sixenseGetNewestData);
	//def("GetData", sixenseGetData);
	def("GetAllNewestData", GetAllNewestData);
	def("GetAllData", GetAllData);
	def("GetNewestData", GetNewestData);
	def("GetData", GetData);
}

