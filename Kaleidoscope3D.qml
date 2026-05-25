import QtQuick
import QtQuick3D
import QtQuick3D.Helpers
import QtQuick3D.Particles3D

Item {
    id: window
    anchors.fill: parent

    property var createdLetters: []

    View3D {
        id: view
        anchors.fill: parent

        environment: ExtendedSceneEnvironment {
            clearColor: "#030307"
            backgroundMode: SceneEnvironment.Color
            antialiasingMode: SceneEnvironment.MSAA
            antialiasingQuality: SceneEnvironment.High

            glowEnabled: true
            glowIntensity: 1.2
            glowStrength: 0.8
            glowBloom: 1.5
        }

        OrbitCameraController {
            id: cameraController
            origin: kaleidoscopeAnchor
            camera: userCamera
            mouseEnabled: true
            panEnabled: false
        }

        PerspectiveCamera {
            id: userCamera
            z: 600
        }

        DirectionalLight {
            brightness: 0.6
            eulerRotation.x: -25
            eulerRotation.y: -40
            color: Qt.rgba(0.7, 0.5, 1.0, 1.0)
            SequentialAnimation on color {
                loops: Animation.Infinite
                ColorAnimation { from: "#ff66cc"; to: "#66ffcc"; duration: 8000; easing.type: Easing.InOutSine }
                ColorAnimation { from: "#66ffcc"; to: "#ff66cc"; duration: 8000; easing.type: Easing.InOutSine }
            }
            SequentialAnimation on eulerRotation.z {
                loops: Animation.Infinite
                NumberAnimation { from: -15; to: 15; duration: 7000; easing.type: Easing.InOutSine }
                NumberAnimation { from: 15; to: -15; duration: 7000; easing.type: Easing.InOutSine }
            }
        }

        Node {
            id: kaleidoscopeAnchor
            NumberAnimation on eulerRotation.z {
                from: 0; to: 360; duration: 20000; loops: Animation.Infinite
            }
        }

        ParticleSystem3D {
            id: particleSystem
            running: true

            ParticleEmitter3D {
                id: emitter
                system: particleSystem
                particle: sparkles
                position: Qt.vector3d(0, 0, 0)
                shape: ParticleShape3D {
                    type: ParticleShape3D.Cube
                    extents: Qt.vector3d(300, 300, 300)
                }
                emitRate: 150
                lifeSpan: 2000
                lifeSpanVariation: 500
                particleScale: 0.2
                particleEndScale: 0.0
                velocity: VectorDirection3D {
                    direction: Qt.vector3d(0, 50, 0)
                    directionVariation: Qt.vector3d(40, 20, 40)
                }
            }

            SpriteParticle3D {
                id: sparkles
                color: "#ffffaa"
                colorVariation: Qt.vector4d(0.5, 0.2, 0.8, 0)
                fadeInDuration: 200
                fadeOutDuration: 400
                hasTransparency: true
            }

            Gravity3D {
                system: particleSystem
                enabled: true
                magnitude: 40
                direction: Qt.vector3d(0, -1, 0)
            }
        }
    }

    Component {
        id: letterTemplate
        Node {
            id: letterNode

            property string text: ""
            property color neonColor: "white"
            property real baseX: 0
            property real baseY: 0
            property real baseZ: 0
            property real offsetX: 0
            property real offsetY: 0
            property real offsetZ: 0
            property real extraRot: 0
            property real scaleFactor: 1.0
            property real glowIntensity: 0.5

            // Коэффициенты амплитуды для анимаций (уменьшаются при длинном тексте)
            property real ampX: 1.0
            property real ampY: 1.0
            property real ampZ: 1.0
            property real ampRot: 1.0
            property real ampScale: 1.0

            x: baseX + offsetX
            y: baseY + offsetY
            z: baseZ + offsetZ
            scale: Qt.vector3d(scaleFactor, scaleFactor, scaleFactor)
            eulerRotation.z: extraRot

            NumberAnimation on glowIntensity {
                from: 0.3; to: 1.0
                duration: Math.random() * 2000 + 1000
                loops: Animation.Infinite
                easing.type: Easing.InOutSine
            }

            Item {
                id: textureSourceItem
                width: 256; height: 256
                visible: false
                Text {
                    id: letterText
                    anchors.centerIn: parent
                    text: letterNode.text
                    font.pointSize: 130
                    font.bold: true
                    color: letterNode.neonColor
                    style: Text.Outline
                    styleColor: "white"
                }
            }

            Texture {
                id: letterTexture
                sourceItem: textureSourceItem
                mipFilter: Texture.Linear
            }

            Model {
                source: "#Rectangle"
                scale: Qt.vector3d(0.52, 0.52, 0.05)
                position: Qt.vector3d(0, 0, -8)
                materials: DefaultMaterial {
                    lighting: DefaultMaterial.NoLighting
                    diffuseColor: Qt.darker(letterNode.neonColor, 1.8)
                    opacity: 0.4 * letterNode.glowIntensity
                    cullMode: DefaultMaterial.NoCulling
                }
            }
            Model {
                source: "#Rectangle"
                scale: Qt.vector3d(0.51, 0.51, 0.05)
                position: Qt.vector3d(0, 0, -4)
                materials: DefaultMaterial {
                    lighting: DefaultMaterial.NoLighting
                    diffuseColor: Qt.darker(letterNode.neonColor, 1.2)
                    opacity: 0.7 * letterNode.glowIntensity
                    cullMode: DefaultMaterial.NoCulling
                }
            }
            Model {
                source: "#Rectangle"
                scale: Qt.vector3d(0.5, 0.5, 0.05)
                materials: DefaultMaterial {
                    lighting: DefaultMaterial.NoLighting
                    diffuseMap: letterTexture
                    blendMode: DefaultMaterial.SourceOver
                    cullMode: DefaultMaterial.NoCulling
                }
            }
            Model {
                source: "#Rectangle"
                scale: Qt.vector3d(0.53, 0.53, 0.05)
                position: Qt.vector3d(0, 0, 3)
                materials: DefaultMaterial {
                    lighting: DefaultMaterial.NoLighting
                    diffuseMap: letterTexture
                    opacity: 0.5 * letterNode.glowIntensity
                    blendMode: DefaultMaterial.SourceOver
                    cullMode: DefaultMaterial.NoCulling
                }
            }

            // Анимации с динамической амплитудой (коэффициенты задаются при создании)
            NumberAnimation on offsetX {
                from: -30 * ampX; to: 30 * ampX
                duration: Math.random() * 3500 + 2000
                loops: Animation.Infinite
                easing.type: Easing.InOutSine
            }
            NumberAnimation on offsetY {
                from: -25 * ampY; to: 25 * ampY
                duration: Math.random() * 4000 + 2500
                loops: Animation.Infinite
                easing.type: Easing.InOutSine
            }
            NumberAnimation on offsetZ {
                from: -35 * ampZ; to: 35 * ampZ
                duration: Math.random() * 4500 + 2500
                loops: Animation.Infinite
                easing.type: Easing.InOutSine
            }
            NumberAnimation on extraRot {
                from: -25 * ampRot; to: 25 * ampRot
                duration: 3500
                loops: Animation.Infinite
                easing.type: Easing.InOutSine
            }
            NumberAnimation on scaleFactor {
                from: 0.8 * ampScale; to: 1.2 * ampScale
                duration: 2200
                loops: Animation.Infinite
                easing.type: Easing.InOutQuad
            }
        }
    }

    function updateText(newText) {
        for (var i = 0; i < createdLetters.length; i++)
            createdLetters[i].destroy();
        createdLetters = [];

        if (newText.length === 0) return;

        var len = newText.length;

        // --- Узкий туннель: радиус меньше ---
        var radius = Math.min(200, 80 + len * 1.2);   // было 450 / 120 + len*3
        var totalTurns = Math.min(6.0, 1.5 + len / 20.0);
        var angleStep = (2 * Math.PI * totalTurns) / len;

        var zSpan = Math.min(1200, 300 + len * 8.0);   // длина осталась
        var zStep = zSpan / len;
        var startZ = -zSpan / 2;

        var amplitudeFactor = Math.max(0.25, 1.0 - len / 150.0);

        for (var j = 0; j < len; j++) {
            var charStr = newText.charAt(j);
            if (charStr === " ") continue;

            var t = j / len;
            var angle = j * angleStep;

            // Убираем расширение радиуса в глубину (оставляем постоянный или чуть-чуть)
            var r = radius;   // было radius * (1 + t*0.2)
            var posX = r * Math.cos(angle);
            var posY = r * Math.sin(angle) + Math.sin(angle * 2.5) * 20 * (1 - t * 0.5);
            var posZ = startZ + j * zStep;

            var rColor = 0.2 + t * 0.8;
            var gColor = 0.2 + Math.sin(t * Math.PI) * 0.6;
            var bColor = 0.8 - t * 0.7;
            var letterColor = Qt.rgba(rColor, gColor, bColor, 1.0);

            var distanceScale = 1.0 - t * 0.4;

            var letter = letterTemplate.createObject(kaleidoscopeAnchor, {
                "text": charStr,
                "baseX": posX,
                "baseY": posY,
                "baseZ": posZ,
                "neonColor": letterColor,
                "ampX": amplitudeFactor,
                "ampY": amplitudeFactor,
                "ampZ": amplitudeFactor,
                "ampRot": amplitudeFactor,
                "ampScale": amplitudeFactor * distanceScale
            });
            createdLetters.push(letter);
        }
    }
}