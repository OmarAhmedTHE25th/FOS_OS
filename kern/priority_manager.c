#include <inc/stdio.h>
#include <kern/priority_manager.h>
#include <inc/assert.h>
#include <kern/helpers.h>
#include <kern/user_environment.h>

void set_program_priority(struct Env* env, int priority)
{

	if (env == NULL)
			return;

		assert(priority >= PRIORITY_LOW && priority <= PRIORITY_HIGH);

		env->priority = priority;

		switch (priority)
		{
		case PRIORITY_LOW:
			half_WS_Size(env,0);
			break;

		case PRIORITY_BELOWNORMAL:
			half_WS_Size(env, 0);
			break;

		case PRIORITY_NORMAL:
			break;

		case PRIORITY_ABOVENORMAL:
			double_WS_Size(env, 1);
			break;

		case PRIORITY_HIGH:
			double_WS_Size(env, 0);
			break;
		}

}
