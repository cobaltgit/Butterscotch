import java.util.Properties

plugins {
    alias(libs.plugins.android.application)
    alias(libs.plugins.kotlin.compose)
    alias(libs.plugins.kotlin.serialization)
}

// Signing settings
val keystorePropertiesFile = rootProject.file("keystore.properties")
val keystoreProperties = Properties().apply {
    if (keystorePropertiesFile.exists()) {
        keystorePropertiesFile.inputStream().use { load(it) }
    }
}

// ⚠️ ainda aponta pro runtime original (não quebrar build agora)
val butterscotchRepoDir = file("../../Butterscotch")

android {
    namespace = "net.perfectdreams.butterscotch.android"
    compileSdk = 36

    defaultConfig {
        applicationId = "net.perfectdreams.butterscotch"
        minSdk = 24
        targetSdk = 36

        versionCode = 1
        versionName = "0.7-spaghetti"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"

        externalNativeBuild {
            cmake {
                arguments += listOf(
                    "-DPLATFORM=android",
                    "-DENABLE_GLES=ON",
                    "-DENABLE_MODERN_GL=ON",
                    "-DENABLE_LEGACY_GL=OFF",
                    "-DAUDIO_BACKEND=miniaudio"
                )
                cppFlags += "-std=c++17"
                cFlags += "-std=gnu99"
            }
        }
    }

    signingConfigs {
        create("release") {
            if (keystorePropertiesFile.exists()) {
                storeFile = file(keystoreProperties["storeFile"] as String)
                storePassword = keystoreProperties["storePassword"] as String
                keyAlias = keystoreProperties["keyAlias"] as String
                keyPassword = keystoreProperties["keyPassword"] as String
            }
        }
    }

    buildTypes {
        release {
            buildConfigField("boolean", "FORCE_SPAGHETTI_PLUS", "false")
            buildConfigField("String", "API_BASE_URL", "\"https://mizzle.spaghetti.gg\"")
            buildConfigField("String", "API_VERSION", "\"v1\"")

            isMinifyEnabled = true
            isShrinkResources = true
            signingConfig = if (keystorePropertiesFile.exists())
                signingConfigs.getByName("release") else null

            ndk {
                debugSymbolLevel = "SYMBOL_TABLE"
                abiFilters += listOf("arm64-v8a", "armeabi-v7a", "x86_64")
            }

            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }

        debug {
            buildConfigField("boolean", "FORCE_SPAGHETTI_PLUS", "true")
            buildConfigField("String", "API_BASE_URL", "\"http://192.168.15.125:8080\"")
            buildConfigField("String", "API_VERSION", "\"v1\"")

            ndk {
                abiFilters += listOf("arm64-v8a")
            }
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }

    buildFeatures {
        compose = true
        buildConfig = true
    }

    externalNativeBuild {
        cmake {
            path = File(butterscotchRepoDir, "CMakeLists.txt")
            version = "3.22.1"
        }
    }
}

/**
 * Removido: GenerateContributorsTask + androidComponents
 * (dependência inexistente quebrava o build)
 */

dependencies {
    implementation(project(":common"))

    implementation("org.jetbrains.kotlinx:kotlinx-serialization-json:1.11.0")

    implementation(platform(libs.androidx.compose.bom))
    implementation(libs.androidx.activity.compose)
    implementation(libs.androidx.compose.material3)
    implementation(libs.androidx.compose.material.icons.core)
    implementation(libs.androidx.compose.material.icons.extended)
    implementation(libs.androidx.compose.ui)
    implementation(libs.androidx.compose.ui.graphics)
    implementation(libs.androidx.compose.ui.tooling.preview)
    implementation(libs.androidx.core.ktx)
    implementation(libs.androidx.core.splashscreen)
    implementation(libs.androidx.documentfile)
    implementation(libs.androidx.navigation.compose)
    implementation(libs.kotlinx.serialization.json)
    implementation(libs.androidx.lifecycle.runtime.ktx)

    implementation("org.jetbrains.kotlinx:kotlinx-coroutines-core:1.11.0")
    implementation("io.ktor:ktor-client-cio:3.5.0")

    implementation("com.google.android.play:app-update:2.1.0")
    implementation("com.google.android.play:app-update-ktx:2.1.0")

    implementation("androidx.fragment:fragment-ktx:1.8.5")

    testImplementation(libs.junit)

    androidTestImplementation(platform(libs.androidx.compose.bom))
    androidTestImplementation(libs.androidx.compose.ui.test.junit4)
    androidTestImplementation(libs.androidx.espresso.core)
    androidTestImplementation(libs.androidx.junit)

    debugImplementation(libs.androidx.compose.ui.test.manifest)
    debugImplementation(libs.androidx.compose.ui.tooling)
}
