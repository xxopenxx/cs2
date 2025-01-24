import requests
import json

def update_offsets():
    url = "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/offsets.json"
    
    try:
        response = requests.get(url)
        data = response.json()
        
        with open("../Common/include/offsets.hpp", "w") as f:
            f.write("#pragma once\n#include <cstdint>\n\nnamespace offsets {\n")
            f.write(f"    constexpr uintptr_t dwEntityList = 0x{data['client.dll']['dwEntityList']:X};\n")
            f.write(f"    constexpr uintptr_t dwLocalPlayerPawn = 0x{data['client.dll']['dwLocalPlayerPawn']:X};\n")
            f.write(f"    constexpr uintptr_t dwViewMatrix = 0x{data['client.dll']['dwViewMatrix']:X};\n")
            f.write("}\n")
            
        print("Offsety zaktualizowane!")
    except Exception as e:
        print(f"Błąd: {str(e)}")

if __name__ == "__main__":
    update_offsets()