#include "HelloWorldExample.h"
#include <string>
#include <iostream>
#include "caf/all.hpp"

// Используем только конкретные классы из std
using std::endl;
using std::string;

caf::message_handler valueReceiver(caf::event_based_actor* self) {
    caf::message_handler x1{
        [](int i) {
        },
        [](double db) {
        },
        [](int a, int b, int c) {
        }
    };
    
    caf::message_handler x2{
        [](double db) {
        },
        [](double db) {
            /* - unreachable - */
        }
    };
    
    // TODO: ???
    caf::message_handler x3 = x1.or_else(x2);
    caf::message_handler x4 = x2.or_else(x1);
    
    caf::atom_value a1 = caf::atom("add");
    caf::atom_value a2 = caf::atom("multiply");
    
    using add_atom = caf::atom_constant<caf::atom("add")>;
    using multiply_atom = caf::atom_constant<caf::atom("multiply")>;
    
    caf::behavior do_math{
        [](add_atom, int a, int b) {
            return a + b;
        },
        [](multiply_atom, int a, int b) {
            return a * b;
        }
    };
    
    return x4;
}

caf::behavior mirror(caf::event_based_actor* self) {
    aout(self) << "Mirror begin" << endl;
    
    // Обработчик в качестве параметра принимает строку и возвращает строку
    auto behaviourMethod = [=](const string& what) -> string {
        // Печатаем входное сообщение потокобезопасно
        aout(self) << what << endl;
        // Отвечаем строкой в обратном порядке
        std::string result = string(what.rbegin(), what.rend());
        return result;
    };
    // Обработчик в качестве параметра принимает строку и возвращает строку
    caf::behavior behaviour(behaviourMethod);
    
    aout(self) << "Mirror end" << endl;
    
    return behaviour;
}

void hello_world(caf::event_based_actor* self, const caf::actor& nextActor, const caf::actor& otherActor) {
    aout(self) << "Hello world begin" << endl;
    
    // Отправляем "Hello World!" другому нашему актору "bubby"
    auto thenCallback = [=](const string& what) {
        // Затем печатаем этот ответ потокобезопасно
        aout(self) << what << endl;
    };
    // Ждем до 10 секунд ответа
    auto responce1 = self->request(nextActor, std::chrono::seconds(10), "Hello World!");
    responce1.then(thenCallback);
    
    aout(self) << "Hello world end" << endl;
}

void hello_world_caf_main(caf::actor_system& system) {
    // Устанавливаем в систему нового актора
    auto mirrorActor = system.spawn(mirror);
    
    // Создаем нового актора c параметром в виде другого актора
    auto helloWorldActor = system.spawn(hello_world, mirrorActor, mirrorActor);
    
    // Система будет ждать пока оба актора завершатся перед выходом из main
}
