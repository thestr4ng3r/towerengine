
#ifndef _MATERIAL_MANAGER_H
#define _MATERIAL_MANAGER_H


/// class for organizing multiple materials by names.
/// All materials added are "owned" by the manager and will be deleted with the manager object.
class tMaterialManager
{
	private:
		std::map<std::string, tMaterial *> materials;

	public:
		tMaterialManager(void);
		~tMaterialManager(void);

		tMaterial *GetMaterial(std::string name);
		void AddMaterial(std::string name, tMaterial *material);
};


#endif
