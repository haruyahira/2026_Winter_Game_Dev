// Src/Object/Magic/GenericMagic.h
#pragma once
#include <memory>
#include "MagicBase.h"
#include "MagicData.h"

class ProjectileComponent;

class GenericMagic : public MagicBase
{
public:
	static constexpr float EXPLODE_DURATION = 0.2f;

	// ê∂ê¨éûÇ…Åuê›åvê}ÅvÇéÛÇØéÊÇÈ
	GenericMagic(const MagicData& data);
	virtual ~GenericMagic(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Fire(const Transform& spawnTransform, VECTOR dir) override;
	void OnHit(void) override;
	bool CanHit(void) const override;
	bool isDestroy(void) override;

private:
	MagicData data_;
	std::shared_ptr<ProjectileComponent> projectileComp_;
	bool isExploding_;
	float explodeTimer_;
};