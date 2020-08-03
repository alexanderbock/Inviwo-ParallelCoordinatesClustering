in flat int identifier;
in flat int nClusters;

uniform int _nData;
uniform bool _hasColoringData;
uniform sampler2D _transFunc;
uniform float _alphaFactor;
uniform bool _excludeZero;

uniform bool _depthTesting;

void main() {
    if (_excludeZero && identifier == 0) {
        discard;
    }

    float alpha = 1.0;
    if (!_depthTesting) {
        alpha = _alphaFactor * 1000.0 / float(_nData);
        alpha = clamp(alpha, 0.0, 0.25);
    }

    if (_hasColoringData) {
        vec4 color = vec4(texture(_transFunc, vec2(float(identifier) / float(nClusters), 0.5)).rgb, alpha);
        FragData0 = color;
    }
    else
        FragData0 = vec4(1.0, 1.0, 1.0, alpha);


    // FragData0 = vec4(1.0, 0.0, 0.0, 1.0);
}
