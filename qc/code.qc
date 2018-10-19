<|qc|>
	public <class> CheckSeparateAllChildren : UnityEngine.MonoBehaviour
		public <func> CheckSeparate( )
			<for> int i = 0; i less transform.childCount; i++
				UnityEngine.Transform child = transform.GetChild( i );
				var s = child.GetComponent{{IShip}}();
				<if> s not equals null && child.gameObject.activeSelf
					s.ChechSeperate();
				</>
			</>
		</>
	</>
</>