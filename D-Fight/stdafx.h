#pragma once

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "freeglut.lib")

// 사운드 라이브러리
#pragma comment(lib, "irrKlang.lib")
// 폰트 라이브러리
#pragma comment(lib, "freetype.lib")
// 모델 로드 라이브러리
#pragma comment(lib, "assimp.lib")

// openGL 관련 라이브러리
#include "Dependencies/glew.h"
#include "Dependencies/freeglut.h"
#include "Dependencies/glm/glm.hpp"
#include "Dependencies/glm/ext.hpp"
#include "Dependencies/glm/gtc/matrix_transform.hpp"
#include "Dependencies/glm/gtc/quaternion.hpp"
#include "Dependencies/glm/gtx/quaternion.hpp"


#include "Dependencies/irrKlang/irrKlang.h"

#include "Dependencies/ft2build.h"
#include FT_FREETYPE_H

#include "Dependencies/assimp/Importer.hpp"
#include "Dependencies/assimp/scene.h"
#include "Dependencies/assimp/postprocess.h"

#include <iostream>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <iomanip>
