#pragma once

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "freeglut.lib")

// ���� ���̺귯��
#pragma comment(lib, "irrKlang.lib")
// ��Ʈ ���̺귯��
#pragma comment(lib, "freetype.lib")
// �� �ε� ���̺귯��
#pragma comment(lib, "assimp.lib")

// openGL ���� ���̺귯��
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
