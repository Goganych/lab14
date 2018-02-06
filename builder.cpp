#include <boost/process.hpp>
#include <boost/filesystem.hpp>
//#include <boost/process/system.hpp>
#include <boost/program_options.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;
namespace bp = boost::process;
namespace po = boost::program_options;

int w_time = 0; //waiting time - local value (for more convenient access in functions)

// ====STEP ONE====
// "create" module -> command $cmake -H. -B_builds -DCMAKE_INSTALL_PREFIX=_install -DCMAKE_BUILD_TYPE=<Debug|Release>
bp::child create(boost::filesystem::path cmake_path, string build = "Debug"){
    
    bp::group g; // for terminate all child processes
    
    string dcmake_build_type = "-DCMAKE_BUILD_TYPE=" + build;
    
    bp::child c(cmake_path, "-H.", "-B_builds", "-DCMAKE_INSTALL_PREFIX=_install", dcmake_build_type, bp::std_out > stdout, g);
    
    if(w_time){
        if (!g.wait_for(chrono::seconds(w_time))){
            g.terminate();
        }
    }
    else{
        g.wait();
    }
    return c;
}

// ====STEP TWO====
// building proj -> command $cmake --build _builds
bp::child build(boost::filesystem::path cmake_path){

    bp::group g;
    bp::child c(cmake_path, "--build", "_builds", bp::std_out > stdout, g);
    
    if(w_time){
        if (!g.wait_for(chrono::seconds(w_time))){
            g.terminate();
        }
    }
    else{
        g.wait();
    }
    
    return c;
}

// ====BUILD WITH TARGET====
bp::child target(boost::filesystem::path cmake_path, string target){
    
    bp::child c(cmake_path, "--build", "_builds", "--target", target, bp::std_out > stdout);
    return c;
}

int main(int argc, char** argv)
{
    po::options_description desc("Usage: builder [options]\nAllowed options");
    desc.add_options()
    ("help,h", ": help info")
    ("config", po::value<std::string>(),  ": build configuration: <Release|Debug>, default - Debug")
    ("install", ": add installation phase")
    ("pack", ": add installation phase")
    ("timeout", po::value<int>(), ": set waiting time (seconds)");
    
    boost::filesystem::path cmake_path = bp::search_path("../bin/cmake");
    
    if (!boost::filesystem::exists(cmake_path)){
        cout << "Файл cmake не существует или не найден" << endl;
        return 0;
    }
    
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    
    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }
    if (vm.count("timeout")){
        
        w_time = vm["timeout"].as<int>();
        cout << "building with timeout" << endl << "timeout is " << w_time << endl;
        
        bp::child c = create(cmake_path);
        int ex = c.exit_code();
        cout << "create exit code is " << ex << endl;
        
        if (!ex){
            bp::child b = build(cmake_path);
            ex = b.exit_code();
            cout << "build exit code is " << ex << endl;
        }
    }
    else if (vm.count("config")){
        
        string param = vm["config"].as<std::string>();
        cout << "used command: $cmake -H. -B_builds -DCMAKE_INSTALL_PREFIX=_install -DCMAKE_BUILD_TYPE=" << param << endl;
        bp::child c = create(cmake_path, param);
        int ex = c.exit_code();
        cout << "command(create) exit code: " << ex << endl;
        
        if(ex){
            cout << "Create exit code is not 0, cannot start build process. Exit." << endl;
            return 0;
        }
        
        cout << "used command: $cmake --build _builds" << endl;
        bp::child b = build(cmake_path);
        ex = b.exit_code();
        cout << "command(build) exit code: " << ex << endl;

//        boost::filesystem::path p = "/usr/bin/g++"; //or get it from somewhere else.
//        int result = bp::system("/usr/local/Cellar/cmake/3.9.3_1/bin/cmake", "-H.", "-B_builds");
//        b_Release.wait();
//        bp::child b_build(cmake_path, "--build", "_builds", g);
//        b_build.wait();
//        bp::child b_Release(cmake_path, "-H.", "-B_builds", "-DCMAKE_INSTALL_PREFIX=_install", "-DCMAKE_BUILD_TYPE=Release", g);

    }
    else if (vm.count("pack") && vm.count("install")){
        
        cout << "used command: $cmake -H. -B_builds -DCMAKE_INSTALL_PREFIX=_install -DCMAKE_BUILD_TYPE=Debug" << endl;
        bp::child c = create(cmake_path);
        int ex = c.exit_code();
        cout << "command(create) exit code: " << ex << endl;
        
        if(ex){
            cout << "Create exit code is not 0, cannot start build process. Exit." << endl;
            return 0;
        }
        
        cout << "used command: $cmake --build _builds" << endl;
        bp::child b = build(cmake_path);
        ex = b.exit_code();
        cout << "command(build) exit code: " << ex << endl;
        
        if(ex){
            cout << "Build exit code is not 0, cannot start install process. Exit." << endl;
            return 0;
        }
        
        cout << "used command: $cmake --build _builds --target install" << endl;
        bp::child ti = target(cmake_path, "install");
        ex = ti.exit_code();
        cout << "command(target install) exit code: " << ex << endl;
        
        if(ex){
            cout << "Install exit code is not 0, cannot start install process. Exit." << endl;
            return 0;
        }
        
        cout << "used command: $cmake --build _builds --target package" << endl;
        bp::child tp = target(cmake_path, "package");
        ex = tp.exit_code();
        cout << "command(target package) exit code: " << ex << endl;
    }
    else if (vm.count("install")){
    
        cout << "used command: $cmake -H. -B_builds -DCMAKE_INSTALL_PREFIX=_install -DCMAKE_BUILD_TYPE=Debug" << endl;
        bp::child c = create(cmake_path);
        int ex = c.exit_code();
        cout << "command(create) exit code: " << ex << endl;
        
        if(ex){
            cout << "Create exit code is not 0, cannot start build process. Exit." << endl;
            return 0;
        }
        
        cout << "used command: $cmake --build _builds" << endl;
        bp::child b = build(cmake_path);
        ex = b.exit_code();
        cout << "command(build) exit code: " << ex << endl;
        
        if(ex){
            cout << "Build exit code is not 0, cannot start install process. Exit." << endl;
            return 0;
        }
        
        cout << "used command: $cmake --build _builds --target install" << endl;
        bp::child t = target(cmake_path, "install");
        ex = t.exit_code();
        cout << "command(target install) exit code: " << ex << endl;
    }
    else if (vm.count("pack")){
        cout << "used command: $cmake -H. -B_builds -DCMAKE_INSTALL_PREFIX=_install -DCMAKE_BUILD_TYPE=Debug" << endl;
        bp::child c = create(cmake_path);
        int ex = c.exit_code();
        cout << "command(create) exit code: " << ex << endl;
        
        if(ex){
            cout << "Create exit code is not 0, cannot start build process. Exit." << endl;
            return 0;
        }
        
        cout << "used command: $cmake --build _builds" << endl;
        bp::child b = build(cmake_path);
        ex = b.exit_code();
        cout << "command(build) exit code: " << ex << endl;
        
        if(ex){
            cout << "Build exit code is not 0, cannot start install process. Exit." << endl;
            return 0;
        }
        
        cout << "used command: $cmake --build _builds --target package" << endl;
        bp::child t = target(cmake_path, "package");
        ex = t.exit_code();
        cout << "command(target package) exit code: " << ex << endl;
    }
    else{
        cout << "used command: $cmake -H. -B_builds -DCMAKE_INSTALL_PREFIX=_install -DCMAKE_BUILD_TYPE=Debug" << endl;
        bp::child c = create(cmake_path);
        int ex = c.exit_code();
        cout << "command(create) exit code: " << ex << endl;
        
        if(ex){
            cout << "Create exit code is not 0, cannot start build process. Exit." << endl;
            return 0;
        }
        
        cout << "used command: $cmake --build _builds" << endl;
        bp::child b = build(cmake_path);
        ex = b.exit_code();
        cout << "command(build) exit code: " << ex << endl;
 
    }
    return 0;
}
