using System;
using System.Collections.Concurrent;
using System.Reflection;
using System.Runtime.CompilerServices;

namespace Coral.Managed;

public class UniqueIdList<T>
{
	private readonly Func<T, int> hashFunction = null;

	private readonly ConcurrentDictionary<int, T> m_Objects = new();

    public UniqueIdList()
    {
        hashFunction = (o) => o?.GetHashCode() ?? 0;
    }

    public UniqueIdList(Func<T, int> hashFunc)
    {
        hashFunction = hashFunc;
    }

    public bool Contains(int id)
	{
		return m_Objects.ContainsKey(id);
	}

	public int Add(T? obj)
	{
		if (obj == null)
		{
			throw new ArgumentNullException(nameof(obj));
		}

		int hashCode = hashFunction(obj);
		_ = m_Objects.TryAdd(hashCode, obj);
		return hashCode;
	}
	public bool TryGetValue(int id, out T? obj)
	{
		return m_Objects.TryGetValue(id, out obj);
	}

	public void Clear()
	{
		m_Objects.Clear();
	}
}
