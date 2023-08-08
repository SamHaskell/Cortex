#include <Cortex.hpp>

class Testbed : public Cortex::App {
    public:
        Testbed() {

        }
        ~Testbed() {
            
        }
    private:
};

Cortex::App *Cortex::CreateApp() {
    return new Testbed();
}