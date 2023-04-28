#include <Cortex.hpp>

class Testbed : public Cortex::Application {
public:
    Testbed() {
        
    }
    ~Testbed() {

    }
private:
};

Cortex::Application* Cortex::CreateApplication () {
    return new Testbed();
}