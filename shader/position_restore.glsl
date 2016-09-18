
layout(std140) uniform PositionRestoreDataBlock
{
	mat4 modelview_projection_matrix_inv;
	vec2 projection_params;

	// only really necessary for VR
	float uv_factor_x;
	float uv_offset_x;
} position_restore_data_uni;

vec3 CalculateWorldPosition(in float depth, in vec2 uv)
{
	vec3 ndc_pos;
	ndc_pos.xy = vec2(position_restore_data_uni.uv_factor_x, 2.0) * (uv + vec2(position_restore_data_uni.uv_offset_x, -0.5));
	ndc_pos.z = 2.0 * depth - 1.0;

	vec4 clip_pos;
	clip_pos.w = position_restore_data_uni.projection_params.x / (ndc_pos.z - position_restore_data_uni.projection_params.y);
	clip_pos.xyz = ndc_pos * clip_pos.w;

	return (position_restore_data_uni.modelview_projection_matrix_inv * clip_pos).xyz;
}
