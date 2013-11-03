#include <string>
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/message_loop.h"
#include "ppapi/utility/threading/simple_thread.h"
#include "ppapi/utility/completion_callback_factory.h"
#include "SDL/SDL.h"
#include "SDL/SDL_nacl.h"
#include "SDL/SDL_gfxPrimitives.h"

#include "logger.h"
#include "main.h"
#include "setup.h"

using namespace std;

class NaclLogger : public Logger {
    public:
        NaclLogger(pp::Instance& instance) : instance(instance) {}

        virtual void Log(const string& msg) {
            instance.LogToConsole(PP_LOGLEVEL_LOG, msg);
        }

    private:
        pp::Instance& instance;
};

namespace {
    void DispatcherFn(pp::MessageLoop& messageLoop, void* data) {
        Main *main = static_cast<Main*>(data);

        main->Dispatch();
    }
}

class Instance : public pp::Instance {
    public:

        explicit Instance(PP_Instance instance) :
            pp::Instance(instance)
        {
            pp::InstanceHandle instance_handle(this);

            main_thread = new pp::SimpleThread(instance_handle);
            logger = new NaclLogger(*this);
            main = new Main(*logger, setup);

            RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE |
                PP_INPUTEVENT_CLASS_KEYBOARD);
        }

        virtual bool Init(uint32_t argc, const char* argn[], const char* argv[]) {
            bool parent_result = pp::Instance::Init(argc, argn, argv);

            if (parent_result) {
               SDL_NACL_SetInstance(pp_instance(), 640, 480);
               SDL_Init(SDL_INIT_VIDEO);

               main_thread->StartWithFunction(DispatcherFn, main);
            }

            return parent_result;
        }

        virtual bool HandleInputEvent(const pp::InputEvent& event) {
            SDL_NACL_PushEvent(event);
            return true;
        }

        virtual ~Instance() {
            main_thread->Join();

            delete main;
            delete logger;
            delete main_thread;
        }

    private:

        pp::SimpleThread* main_thread;

        NaclLogger* logger;
        Main* main;
        Setup setup;
};

class OurModule: public pp::Module {
    public:
    
        OurModule() : pp::Module() {}

        virtual ~OurModule() {}

        virtual pp::Instance* CreateInstance(PP_Instance instance) {
            return new Instance(instance);
        }
};

namespace pp {
    Module* CreateModule() {
        return new OurModule();
    }
}
